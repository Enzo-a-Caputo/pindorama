#ifndef PINDORAMA_PRODUCE_HPP
#define PINDORAMA_PRODUCE_HPP

#include "Item.hpp"

namespace pindorama {
    /**
 * @brief Um item colhido (cultura ou produto animal), transportado para ser vendido
 *        ou usado em outro lugar. Ele não tem efeito sobre um tile.
 *
 * useOn() sempre recusa: obrigar toda subclasse de Item a implementar uma
 * ação de tile é um indício de design (smell) que vale a pena levantar com a equipe —
 * Produce é a classe que expõe isso. A alternativa (mover useOn para fora de Item
 * para uma interface mais restrita implementada apenas por Seed, Fertilizer
 * e Tool) altera a hierarquia além deste bloco, o que exige a
 * concordância de quem mais mexer em Item.
 */
class Produce : public Item {
public:
    Produce(std::string id, std::string name, std::string description,
            int sellValue);

    /// @brief Sempre uma recusa: o produto colhido não tem ação sobre um tile.
    ActionResult useOn(Tile& tile) override; //conferir isso aqui
};

} // namespace pindorama

#endif