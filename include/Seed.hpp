#ifndef SEED_HPP
#define SEED_HPP
#include <string>

#include "Item.hpp"

namespace pindorama {
    /**
 * @brief Uma semente que pode ser plantada em um tile arado e vazio.
 */
class Seed : public Item {
public:
/**
     * @param cropTypeId Identificador do tipo de colheita (CropType) em que esta semente se desenvolve.
     *        Armazenado como um id simples, e não como uma referência a CropType,
     *        para que este cabeçalho não tenha dependência com o bloco Cultivo.
     *        Quem planta a semente (Tile ou Crop) resolve este id
     *        através do CropRegistry.
     */
    Seed(std::string id, std::string name, std::string description,
         int price, std::string cropTypeId);

    /// @brief Identificador do tipo de colheita em que esta semente se desenvolve.
    const std::string& cropTypeId() const;

    /**
     * @return Recusa se o tile não estiver arado ou já tiver uma
     *         planta; sucesso se a semente criar raiz.
     */
    ActionResult useOn(Tile& tile) override; // conferir!

private:
    std::string cropTypeId_;
};

} // namespace pindorama

#endif