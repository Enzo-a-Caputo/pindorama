#ifndef PINDORAMA_GAMEVIEW_HPP
#define PINDORAMA_GAMEVIEW_HPP

#include "raylib.h"
#include "Game.hpp"

namespace pindorama {

/**
 * @brief Draws the world (tiles, camera, player). The only class that uses raylib.
 *
 * Reads the Game only through const references: it never changes the world.
 * Copying is forbidden because it owns GPU textures (RAII).
 */
class GameView {
public:
    /**
     * @brief Creates the view and loads its textures.
     * @param game Game to draw.
     * @pre A raylib window is already open; game outlives this view.
     */
    explicit GameView(const Game& game);

    /** @brief Unloads the textures. */
    ~GameView();

    GameView(const GameView&) = delete;
    GameView& operator=(const GameView&) = delete;

    /** @brief Draws the current frame. */
    void draw();

private:
    const Game& game_;
    Camera2D camera_;
    Texture2D tileset_;
};

} // namespace pindorama

#endif // PINDORAMA_GAMEVIEW_HPP