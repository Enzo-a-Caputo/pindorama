#ifndef TOOL_HPP
#define TOOL_HPP
#include "Item.hpp"

namespace pindorama {
    /**
 * @brief Uma ferramenta reutilizável. Nunca é consumida pelo uso (preço/valor de venda
 *        da classe Item tipicamente são 0 / não vendizíeis para ferramentas, mas são mantidos
 *        já que todo Item os possui).
 *
 * Ferramentas com seu próprio estado por instância (como as doses restantes do regador)
 * não devem ser espremidas nesta classe com um enum; veja WateringCan.hpp para esse caso.
 * Tool.hpp aqui destina-se a ferramentas simples e sem estado, como a enxada.
 */
class Tool : public Item {
public:
    Tool(std::string id, std::string name, std::string description);

    /**
     * @return Recusa se o tile alvo não for terra (para uma enxada) ou for
     *         incompatível de outra forma; sucesso se for arado.
     */
    ActionResult useOn(Tile& tile) override;
};

} // namespace pindorama

#endif
