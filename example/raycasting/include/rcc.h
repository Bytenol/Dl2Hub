/**
 * @file rcc.h
 * @date 04-oct-2024
 * A single header only library for pseudo3d raycasting. Just drag
 * the header file into your include path and that's all
 * 
 * There are major issues regarding casted ray in this version,
 * Hopefully it'll be fixed very soon
 */
#ifndef __BYTENOL_RAYCASTING3D_WORLD_H__
#define __BYTENOL_RAYCASTING3D_WORLD_H__

#include <memory>
#include <vector>
#include <cmath>
#include <numbers>
#include <cassert>


namespace rcc
{

    class World;
    class Vector;
    class RayCastable;

    using WorldPtr = std::unique_ptr<World>;

    // PI constant
    const double PI = 3.14159f;

    /// @brief Convert from degrees to radians
    /// @param f is the value in degrees
    /// @return the number in radians
    float degToRad(const float& f);


    /// @brief Convert from radians to degrees
    /// @param f is the value in radians
    /// @return the number in degrees
    float radToDeg(const float& f);


    /// @brief Create a pointer instance for the raycasting world
    /// @param tileSize is the size of each tile in the world
    /// @param size is the actual size for the world in screen space
    /// @return a pointer to the created world
    WorldPtr createWorld(const int& tileSize, const Vector& size);


    // 2D vector operations
    struct Vector
    {
        float x = 0.0f;
        float y = 0.0f;
        Vector(const float& _x = 0.0f, const float& _y = 0.0f);
        Vector operator+(const Vector& v) const;
        Vector& operator+=(const Vector& v);

        Vector operator-(const Vector& v) const;
        Vector& operator-=(const Vector& v);

        Vector operator*(const float& s) const;
        Vector& operator*=(const float& s);

        float dotProduct(const Vector& v) const;

        float getLength() const;


        /// @brief Create a vector from a given angle
        /// @param angleInRadians is the angle in radians
        /// @param scale is the scale of the vector, default to 1
        /// @return a Vector instance
        static Vector fromAngle(const float& angleInRadians, const float& scale = 1.0f);
    };


    struct Ray
    {
        float angle = 0.0f; // in degrees
        float dist = 0.0f;
        int tileSize = 0.0f;
        bool isVert = false;
        Vector start, end, dir, xIntercept, yIntercept;

        public:
            Ray(const float& angle);
    };


    /// This is the class for all entities that can cast a ray
    class RayCastable
    {
        private:
            float rayInc = 0.0f;        // incrementation steps between rays
            float fov = 60.0f;          // in degrees
            std::vector<Ray> rays;

            /// @brief Cast rays and check for horizontal intersection with blocks
            /// @param world is the current world
            /// @return a Vector for the overlapped distance before intersection
            /// A ray can take any combination of isLeft and isRight
            /// @todo make blocks customizable: for now a block B is B != 0
            Vector getRayHorizontalIntercept(const float &angleInRadian, const World& world) const;

            /// @brief Cast rays and check for vertical intersection with blocks
            /// @param world is the current world
            /// @return a Vector for the overlapped distance before intersection
            /// A ray can take any combination of isLeft and isRight
            /// @todo make blocks customizable: for now a block B is B != 0
            Vector getRayVerticalIntercept(const float &angleInRadian, const World& world) const;

        public:
            Vector pos;
            Vector vel;
            float rotation = 0.0f;     // in degrees
            RayCastable() = default;

            /// @brief Create an instance of a lifeable character
            /// @param fov is the field of view of the character in degrees
            /// @param rotation is the rotation of the character in degrees
            /// @param fovDiv is the amount that the field of view should be divided into
            RayCastable(float fov, float rotation, int fovDiv = 3);

            std::vector<Ray>& getRays();

            void castRay(const World& world);
    };


    //Principal class for the raycasted world
    class World
    {
        friend WorldPtr createWorld(const int& tileSize, const Vector& size);

        private:

            /// @brief Create an instance of rcc::World where the main logic happens
            /// @param tileSize is an integer representing the size of the tile
            /// @param _size is a Vector representing the width and height of the World
            World(const int& tileSize, const Vector& _size);

            std::vector<RayCastable> rayCastables;

        public:

            void addCastable(const RayCastable& entity);

            void clearCastable();

            std::vector<RayCastable>& getCastables();

            /// @brief Set basic info for the world
            /// @param map is the 1-d tilemap for the world
            /// @param col is the size of the column in the map
            /// @param row is the size of the row in the map
            void setWorldInfo(const std::vector<int>& map, const unsigned& col, const unsigned& row);
            
            /// @brief Set the player for this world
            /// @param player is a pointer to the player
            void setPlayer(RayCastable& player);

            const Vector& getSize() const;

            int getMapId(const int& y, const int& x) const;

            const int& getRowSize() const;

            const int& getColSize() const;

            const int& getTileSize() const;

            void update(float dt);

        private:
            Vector size;
            int rowSize = 0;
            int colSize = 0;
            int tileSize = 0;
            
            const std::vector<int>* currMap = nullptr;
            RayCastable* player;
    };


    Vector::Vector(const float& _x, const float& _y)
    {
        x = _x;
        y = _y;
    }

    
    inline Vector Vector::operator+(const Vector &v) const
    {
        return { x + v.x, y + v.y };
    }
    
    
    inline Vector &Vector::operator+=(const Vector &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }


    inline Vector Vector::operator-(const Vector &v) const
    {
        return { x - v.x, y - v.y };
    }
    
    
    inline Vector &Vector::operator-=(const Vector &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }


    inline Vector Vector::operator*(const float &s) const
    {
        return { x * s, y * s };
    }
    
    
    inline Vector &Vector::operator*=(const float &s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    
    
    inline float Vector::dotProduct(const Vector &v) const
    {
        return x * v.x + y * v.y;
    }

    inline float Vector::getLength() const
    {
        return std::hypot(x, y);
    }

    inline Vector Vector::fromAngle(const float &angleInRadians, const float &scale)
    {
        return Vector{ std::cos(angleInRadians) * scale, std::sin(angleInRadians) * scale };
    }


    inline Ray::Ray(const float& angle)
    {
        this->angle = angle;
    }

    
    inline Vector RayCastable::getRayHorizontalIntercept(const float &angleInRadian, const World& world) const
    {
        float dy, dx, hyp;
        int id, tx, ty;
        Vector dir = Vector::fromAngle(angleInRadian);
        Vector currPos{ 0, 0 };

        bool isLeft = dir.dotProduct(Vector{-1, 0}) >= 0.0f;
        bool isUp = dir.dotProduct(Vector{0, -1}) >= 0.0f;

        short isLeftScaled = isLeft ? -1 : 1;
        short isUpScaled = isUp ? -1 : 1;

        float xOffset = pos.x - std::floor(pos.x / world.getTileSize()) * world.getTileSize();

        auto moveAndCheckId = [&]() -> void {
            currPos.y += dy * isUpScaled;
            currPos.x += dx * isLeftScaled;

            ty = std::floor((pos.y + currPos.y) / world.getTileSize());
            if(isLeft)
                tx = std::floor((pos.x + currPos.x) / world.getTileSize()) - 1;
            else
                tx = std::ceil((pos.x + currPos.x) / world.getTileSize());
            id = world.getMapId(ty, tx);
        };

        dx = isLeft ? xOffset: world.getTileSize() - xOffset;
        dy = std::abs(dx * std::tan(angleInRadian));
        assert(dy >= 0 && dx >= 0);

        moveAndCheckId();
        if(id == 0) {
            dx = world.getTileSize();
            dy = std::abs(dx * std::tan(angleInRadian));
            while (id == 0)
                moveAndCheckId();
        }
        return currPos;
    }

    
    inline Vector RayCastable::getRayVerticalIntercept(const float &angleInRadian, const World &world) const
    {
        float dy, dx, hyp;
        int id;
        Vector dir = Vector::fromAngle(angleInRadian);
        Vector currPos{ 0, 0 };
        Vector currIndex{ 0, 0 };

        bool isLeft = dir.dotProduct(Vector{-1, 0}) >= 0.0f;
        bool isUp = dir.dotProduct(Vector{0, -1}) >= 0.0f;
        short isLeftScaled = isLeft ? -1 : 1;
        short isUpScaled = isUp ? -1 : 1;

        float yOffset = pos.y - std::floor(pos.y / world.getTileSize()) * world.getTileSize();

        const float angInDeg = std::abs(radToDeg(angleInRadian));
        const bool angIsInfinite = angInDeg == 90 || angInDeg == 270;

        auto moveAndCheckId = [&]() -> void {
            currPos.y += dy * isUpScaled;
            currPos.x += dx * isLeftScaled;

            currIndex.x = std::floor((pos.x + currPos.x) / world.getTileSize());
            if(isUp)
                currIndex.y = std::floor((pos.y + currPos.y) / world.getTileSize()) - 1;
            else
                currIndex.y = std::ceil((pos.y + currPos.y) / world.getTileSize());
            id = world.getMapId(currIndex.y, currIndex.x);
        };

        dy = isUp? yOffset : world.getTileSize() - yOffset;
        dx = std::abs(dy / std::tan(angleInRadian));
        if(angIsInfinite) dx = 0;
        assert(dy >= 0 && dx >= 0);

        moveAndCheckId();
        if(id == 0) {
            dy = world.getTileSize();
            dx = isUp? std::abs(dy / std::tan(angleInRadian)): std::abs(dy * std::tan(angleInRadian));
            if(angIsInfinite) dx = 0;
            while (id == 0)
                moveAndCheckId();
        }

        return currPos;
    }

    inline RayCastable::RayCastable(float fov, float rotation, int fovDiv) 
    {
        rayInc = fov / fovDiv;
        const float fovHalf = fov / 2;
        for(float i = -fovHalf; i < fovHalf; i += rayInc)
            rays.push_back({ i });
        
        if(fovDiv == 1) {
            rays.clear();
            rays.push_back(rotation);
        }

        this->fov = fov;
    }


    inline std::vector<Ray> &RayCastable::getRays()
    {
        return rays;
    }

    
    inline void RayCastable::castRay(const World &world)
    {
        for(auto it = rays.begin(); it != rays.end(); it++)
        {
            float angle = degToRad(it->angle + rotation);
            it->dir = Vector::fromAngle(angle);
            it->start = pos;
            it->xIntercept = getRayHorizontalIntercept(angle, world);
            it->yIntercept = getRayVerticalIntercept(angle, world);
            auto& minIntercept = it->xIntercept.getLength() < it->yIntercept.getLength() ? it->xIntercept : it->yIntercept;
            it->isVert = !(&minIntercept == &it->xIntercept);
            it->dist = std::cos(angle) * minIntercept.getLength();
            it->end = pos + it->dir * minIntercept.getLength();
        }
    }

    float degToRad(const float& f)
    {
        return f * PI / 180;
    }

    float radToDeg(const float &f)
    {
        return f * 180 / PI;
    }

    
    WorldPtr createWorld(const int &tileSize, const Vector &size)
    {
        std::unique_ptr<World> world(new World{ tileSize, size });
        return world;
    }


    inline int World::getMapId(const int &y, const int &x) const
    {
        int res;
        try {
            res = currMap->at(y * colSize + x);
        } catch(...) {
            res = -1;
        }

        return res;
    }

    inline const int &World::getRowSize() const
    {
        return rowSize;
    }

    inline const int &World::getColSize() const
    {
        return colSize;
    }

    inline const int &World::getTileSize() const
    {
        return tileSize;
    }

    inline void World::update(float dt)
    {
        player->castRay(*this);
        for(auto it = rayCastables.begin(); it != rayCastables.end(); it++)
            it->castRay(*this);
    }

    inline World::World(const int &tileSize, const Vector &_size)
    {
        this->tileSize = tileSize;
        size = _size;
    }

    inline void World::addCastable(const RayCastable &entity)
    {
        rayCastables.push_back(entity);
    }

    inline void World::clearCastable()
    {
        rayCastables.clear();
    }

    inline std::vector<RayCastable> &World::getCastables()
    {
        return rayCastables;
    }

    inline void World::setWorldInfo(const std::vector<int> &map, const unsigned &col, const unsigned &row)
    {
        currMap = &map;
        colSize = col;
        rowSize = row;
    }


    inline void World::setPlayer(RayCastable &player)
    {
        this->player = &player;
    }


    inline const Vector &World::getSize() const
    {
        return size;
    }

}


#endif