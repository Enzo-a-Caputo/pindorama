#ifndef PINDORAMA_ACTIONRESULT_HPP
#define PINDORAMA_ACTIONRESULT_HPP

#include <string>

namespace pindorama{
      /**
     * @brief Result of a player command: success, or failure with a reason.
     */
    class ActionResult{
    private:
        bool _ok;
        std::string _reason;

        
        ActionResult(bool ok, const std::string& reason);
    
    public:
        static ActionResult ok();
        static ActionResult fail(const std::string& reason);
    
        bool isOk() const;
        const std::string& reason() const;
    };
}
#endif