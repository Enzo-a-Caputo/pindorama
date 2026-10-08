#ifndef PINDORAMA_ITEM_HPP
#define PINDORAMA_ITEM_HPP

#include <string>

namespace pindorama {

// ActionResult (a ver com o Integrante 1) e Tile
// (Integrante 3) estao sendo escritos paralelamente com este header,
// Então o nosso Item.hpp nao esta os incluindo neste escopo agora.

class ActionResult;
class Tile;

/// @brief Discrimina os quatro tipos concretos de Item.
enum class ItemType {
    Seed,
    Fertilizer,
    Tool,
    Produce
};

/**
 * @brief Classe base para todo objeto que o jogador pode carregar ou usar.
 *
 * Item é a raiz polimórfica da hierarquia de itens (Seed, Fertilizer,
 * Tool, Produce). Ele nunca é armazenado por valor em nenhuma parte do projeto —
 * veja Inventory, que possui itens por meio de std::unique_ptr<Item> para
 * evitar o fatiamento de objetos.
 *
 * Item nunca guarda um sprite ou ícone: GameView busca a imagem a
 * ser desenhada através de id().
 */
class Item {
public:
    virtual ~Item() = default;

    // Não copiável: um Item possui um único dono por vez (veja
    // Inventory). A movimentação é permitida para que a posse possa ser transferida
    // explicitamente (ex: mover um item para fora de um slot).
    Item(const Item&) = delete;
    Item& operator=(const Item&) = delete;
    Item(Item&&) = default;
    Item& operator=(Item&&) = default;

    /// @brief Identificador estável, usado para salvar/carregar e buscar ícone.
    const std::string& id() const;

    /// @brief Nome exibido para o jogador.
    const std::string& name() const;

    /// @brief Descrição curta exibida para o jogador (ex: na loja).
    const std::string& description() const;

    /// @brief Qual tipo concreto de item este é.
    ItemType type() const;

    /// @brief Preço de compra na loja. 0 se não for possível comprá-lo lá.
    int price() const;

    /// @brief Valor de venda na caixa de remessa. Significa algo apenas se isSellable().
    int sellValue() const;

    /// @brief Se este item pode ou não ser depositado na caixa de remessa.
    bool isSellable() const;

    /**
     * @brief Aplica o efeito deste item em um tile (plantar, fertilizar,
     *        arar, regar ou colher, dependendo da subclasse
     *        concreta).
     * @param tile Tile em que o jogador está agindo.
     * @return ActionResult descrevendo o sucesso ou o motivo pelo qual a ação
     *         foi recusada (ex: tile em um estado incompatível).
     * @pre nenhum.
     * @post Em caso de sucesso, o estado do tile muda de acordo com o item
     *        concreto e, quando aplicável, este item é consumido pelo
     *        chamador (Inventory), nunca pelo próprio useOn(). Em caso de recusa,
     *        o tile permanece inalterado e nada é consumido.
     */
    virtual ActionResult useOn(Tile& tile) = 0;


protected:
    Item(std::string id, std::string name, std::string description,
         ItemType type, int price, int sellValue, bool sellable);
 
private:
    std::string id_;
    std::string name_;
    std::string description_;
    ItemType type_;
    int price_;
    int sellValue_;
    bool sellable_;

};




} // namespace

#endif