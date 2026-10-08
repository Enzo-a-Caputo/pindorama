#ifndef PINDORAMA_FERTILIZER_HPP 
#define PINDORAMA_FERTILIZER_HPP

#include "Item.hpp"

namespace pindorama {
    /**
 * @brief Acelera o progresso de uma cultura em crescimento em direção ao seu próximo estágio.
 */
class Fertilizer : public Item {
public:
    /**
     * @param growthBonusPercent Porcentagem (ex: 25 para 25%) pela qual
     *        o tempo restante para o próximo estágio da cultura é reduzido.
     */
    Fertilizer(std::string id, std::string name, std::string description,
               int price, int growthBonusPercent);
 
    int growthBonusPercent() const;
 
    /**
     * @return Recusa se o tile não tiver nenhuma planta crescendo ou já estiver
     *         fertilizado; sucesso caso contrário.
     */
    ActionResult useOn(Tile& tile) override;
 
private:
    int growthBonusPercent_;
};
 
} // namespace pindorama





#endif


