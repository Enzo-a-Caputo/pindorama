#ifndef PINDORAMA_TILEMAP_HPP
#define PINDORAMA_TILEMAP_HPP

#include <cstddef>
#include <vector>

#include "Tile.hpp"          // Integrante 3
#include "TimeObserver.hpp"  // Integrante 1 (GameMinutes + TimeObserver)

namespace pindorama {

/**
 * @brief Position in world units (pixels), as used by the player.
 */
struct WorldPosition {
    float x;  ///< Horizontal coordinate.
    float y;  ///< Vertical coordinate.
};

/**
 * @brief Integer coordinate of a tile in the map grid.
 */
struct TileCoord {
    int col;  ///< Column (0 is the leftmost).
    int row;  ///< Row (0 is the topmost).

    /** @brief Two coordinates are equal when column and row match. */
    bool operator==(const TileCoord& other) const noexcept {
        return col == other.col && row == other.row;
    }
    bool operator!=(const TileCoord& other) const noexcept { return !(*this == other); }
};

/**
 * @brief Kind of terrain of a map cell, used for walkability.
 *
 * Finer than Tile's TileKind (which only says soil / non-soil): the map needs
 * to tell a walkable path apart from water or a fence. The mapping is fixed:
 * Soil <-> TileKind::Soil, every other value <-> TileKind::NonSoil.
 */
enum class TerrainType {
    Soil,       ///< Farmable soil; the player can walk on it.
    Path,       ///< Non-farmable but walkable (e.g. a dirt path).
    Water,      ///< Water; blocks movement and cannot be farmed.
    Fence,      ///< Fence; blocks movement and cannot be farmed.
    Structure   ///< Building or other structure; blocks movement and cannot be farmed.
};

/**
 * @brief Inclusive rectangle of tile coordinates (used to draw only visible tiles).
 */
struct TileRect {
    int minCol;  ///< First column (inclusive).
    int minRow;  ///< First row (inclusive).
    int maxCol;  ///< Last column (inclusive).
    int maxRow;  ///< Last row (inclusive).
};

/**
 * @brief Grid of tiles that makes up the farm (CRC card: Fazenda).
 *
 * Coordinate system: orthogonal top-down grid with the origin (0, 0) at the
 * top-left corner of the map, column growing to the right and row growing
 * downwards (the same orientation as raylib's screen space, so no axis flip
 * is needed). Tile (col, row) covers the world area
 * [col * tileSize, (col + 1) * tileSize) x [row * tileSize, (row + 1) * tileSize).
 *
 * Storage: a single contiguous std::vector<Tile> in row-major order, with
 * index = row * width + col. This avoids the extra indirection and
 * allocations of a vector of vectors, keeps hour-by-hour updates cache
 * friendly, and gives O(1) access by coordinate.
 *
 * The TileMap owns every Tile by value and observes the GameClock: on each
 * tick/new day it forwards the event to all tiles (tiles do not register
 * themselves). It knows nothing about rendering and does not include raylib,
 * so coordinate conversion and walkability can be unit-tested without a window.
 *
 * A TileMap is an instance, not a singleton, so other maps may exist later.
 */
class TileMap : public TimeObserver {
public:
    /**
     * @brief Builds a map of @p width x @p height cells, all farmable soil.
     *
     * Each tile is built in place as Tile(row, col, TileKind::Soil): note that
     * Tile takes row first, while TileCoord is {col, row}.
     * @param width Number of columns.
     * @param height Number of rows.
     * @param tileSize Side of one tile in world units.
     * @param shippingBinCoord Coordinate of the shipping bin.
     * @pre width > 0, height > 0, tileSize > 0.
     * @pre shippingBinCoord lies inside the grid.
     * @post getTile() is valid for every coordinate inside the grid.
     * @post Every cell is TerrainType::Soil and holds a Tile at its own
     *       (row, col), in its initial state.
     * @throws std::invalid_argument If any precondition is violated.
     */
    TileMap(int width, int height, int tileSize, TileCoord shippingBinCoord);

    /** @brief Virtual destructor (TimeObserver is polymorphic). */
    ~TileMap() override = default;

    /** @brief Copying is forbidden: the clock holds a pointer to this observer. */
    TileMap(const TileMap&) = delete;
    TileMap& operator=(const TileMap&) = delete;

    /** @return Number of columns. */
    int getWidth() const noexcept;

    /** @return Number of rows. */
    int getHeight() const noexcept;

    /** @return Side of one tile in world units. */
    int getTileSize() const noexcept;

    /**
     * @brief Converts a world position to the coordinate of the tile that contains it.
     * @param position Position in world units; may lie outside the map.
     * @return The tile coordinate (floor division, so negative positions give
     *         negative coordinates). The result may be outside the grid; use isInside().
     * @post No state is changed.
     */
    TileCoord toTileCoord(WorldPosition position) const noexcept;

    /**
     * @brief World position of the top-left corner of a tile.
     * @param coord Tile coordinate; may be outside the grid.
     * @return (col * tileSize, row * tileSize).
     */
    WorldPosition toWorldPosition(TileCoord coord) const noexcept;

    /**
     * @brief World position of the center of a tile (useful for "close enough
     *        to interact" distance checks).
     * @param coord Tile coordinate; may be outside the grid.
     * @return toWorldPosition(coord) plus half a tile in both axes.
     */
    WorldPosition getTileCenter(TileCoord coord) const noexcept;

    /**
     * @brief Range of tiles that intersect a rectangular view of the world,
     *        clamped to the grid. The view draws only this range (culling),
     *        instead of every tile of the map.
     * @param topLeft World position of the view's top-left corner.
     * @param viewWidth Width of the view in world units.
     * @param viewHeight Height of the view in world units.
     * @pre viewWidth >= 0 and viewHeight >= 0.
     * @return Inclusive range inside the grid. If the view does not touch the
     *         map, the range is empty (minCol > maxCol or minRow > maxRow).
     */
    TileRect getVisibleRange(WorldPosition topLeft, float viewWidth, float viewHeight) const noexcept;

    /**
     * @brief Tells whether a coordinate belongs to the grid.
     * @param coord Coordinate to test.
     * @return true if 0 <= col < width and 0 <= row < height.
     */
    bool isInside(TileCoord coord) const noexcept;

    /**
     * @brief Tells whether the player can walk onto a coordinate.
     * @param coord Coordinate to test.
     * @return false if outside the map, if the terrain is water, fence or
     *         structure, or if it is the shipping bin; true for soil and
     *         path cells otherwise.
     */
    bool isWalkable(TileCoord coord) const;

    /**
     * @brief Tells whether farming actions (plow, water, sow, fertilize,
     *        harvest) may be applied to a cell.
     *
     * Tile::plow(), water() and fertilize() have the precondition
     * Tile::isSoil(); the Game calls this first, so it also rejects
     * out-of-map coordinates and the shipping bin cell.
     * @param coord Coordinate to test.
     * @return true if the cell is inside the map, its terrain is
     *         TerrainType::Soil and it is not the shipping bin cell.
     */
    bool isFarmable(TileCoord coord) const;

    /**
     * @brief Terrain of a cell.
     * @param coord Coordinate of the cell.
     * @pre isInside(coord).
     * @return The terrain type.
     * @throws std::out_of_range If the coordinate is outside the grid.
     */
    TerrainType getTerrain(TileCoord coord) const;

    /**
     * @brief Sets the terrain of a cell (used when building the map layout,
     *        before the game starts).
     *
     * Tile fixes its kind at construction, so the cell's Tile is rebuilt as
     * Tile(row, col, kind) with kind derived from @p terrain: any crop or soil
     * state it held is discarded.
     * @param coord Coordinate of the cell.
     * @param terrain New terrain.
     * @pre isInside(coord).
     * @pre The cell is not the shipping bin coordinate.
     * @post getTerrain(coord) == terrain.
     * @post getTile(coord).isSoil() == (terrain == TerrainType::Soil), in its
     *       initial state.
     * @throws std::out_of_range If the coordinate is outside the grid.
     * @throws std::invalid_argument If @p coord is the shipping bin cell.
     */
    void setTerrain(TileCoord coord, TerrainType terrain);

    /**
     * @brief Gives mutable access to a tile (used by the Game when executing commands).
     * @param coord Coordinate of the tile.
     * @pre isInside(coord).
     * @return Reference to the tile; valid as long as the map lives.
     * @throws std::out_of_range If the coordinate is outside the grid.
     */
    Tile& getTile(TileCoord coord);

    /** @copydoc getTile(TileCoord) */
    const Tile& getTile(TileCoord coord) const;

    /** @return Coordinate of the shipping bin. */
    TileCoord getShippingBinCoord() const noexcept;

    /**
     * @brief Forwards the passage of one game tick (10 game minutes) to every tile.
     * @param now Current game time, already advanced by the tick.
     * @param tickMinutes Game minutes represented by one tick (e.g. 10).
     * @post Every tile received exactly one onTick(now, tickMinutes).
     */
    void onTick(GameMinutes now, int tickMinutes) override;

    /**
     * @brief Forwards the start of a new day to every tile.
     * @param now Instant at which the new day starts.
     * @post Every tile received exactly one onNewDay(now).
     */
    void onNewDay(GameMinutes now) override;

private:
    /** @brief Row-major index of a coordinate. @pre isInside(coord). */
    std::size_t indexOf(TileCoord coord) const noexcept;

    int width_;
    int height_;
    int tileSize_;
    TileCoord shippingBinCoord_;
    std::vector<Tile> tiles_;           ///< Farming state, row-major; built with emplace_back(row, col, kind).
    std::vector<TerrainType> terrain_;  ///< Terrain, same indexing as tiles_.
};

}  // namespace pindorama

#endif  // PINDORAMA_TILEMAP_HPP
