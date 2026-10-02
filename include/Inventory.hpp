#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <string>
#include <memory>
#include <vector>


namespace pindorama {
class Item;
class ActionResult;

/**
 * @brief Guarda os itens que o jogador está carregando.
 *
 * Os itens são possuídos através de std::unique_ptr<Item>, nunca armazenados por
 * valor: Item é uma classe base polimórfica, então armazená-la por valor
 * fatiaria (object slicing) toda Seed/Fertilizer/Tool/Produce reduzindo-a a um simples Item.
 *
 * Regra de empilhamento proposta aqui (confirmar com a equipe antes de
 * publicar): Seed, Fertilizer e Produce compartilham um slot até
 * stackLimit(); Tool nunca empilha (cada instância ocupa seu próprio slot,
 * a quantidade é sempre 1), já que duas ferramentas não são intercambiáveis da
 * mesma forma que duas unidades da mesma semente são.
 */
class Inventory {
public:
    explicit Inventory(int capacity);

    /// @brief Número de slots ocupados.
    int slotCount() const;

    /// @brief Número máximo de slots.
    int capacity() const;

    /// @brief Se não há slot livre e nenhuma pilha com espaço.
    bool isFull() const;

    /// @brief Item atualmente equipado pelo jogador, ou nullptr se não houver nenhum.
    Item* selectedItem() const;

    /**
     * @brief Equipa o item no slot fornecido para a próxima ação.
     * @pre slotIndex deve conter um item.
     */
    ActionResult selectSlot(int slotIndex);

    /**
     * @brief Adiciona uma unidade de um item, empilhando-o com um
     *        slot existente do mesmo id quando possível.
     * @param item Item a ser adicionado; a posse é transferida em caso de sucesso.
     * @return sucesso, ou recusa se não houver slot livre e nenhuma
     *         pilha existente com espaço (o núcleo apenas retorna o resultado;
     *         ele nunca exibe uma mensagem por si só — veja ActionResult).
     */
    ActionResult addItem(std::unique_ptr<Item> item);

    /**
     * @brief Remove uma ou mais unidades do item com este id.
     * @pre o inventário deve conter pelo menos a quantidade (quantity) de unidades de itemId.
     */
    ActionResult removeItem(const std::string& itemId, int quantity = 1);

    /// @brief Se o jogador possui pelo menos uma unidade deste item.
    bool hasItem(const std::string& itemId) const;

    /// @brief Se caberiam mais N unidades deste item neste exato momento.
    bool canFit(const std::string& itemId, int quantity) const;

private:
    struct Slot {
        std::unique_ptr<Item> item;
        int quantity;
    };

    int capacity_;
    int selectedSlot_;
    std::vector<Slot> slots_;
};

} // namespace pindorama


#endif