#include "stdafx.h"
#include "ForCRule.h"

#include "Rules/Operators/ForC/ForC.h"

BackusRulePtr MakeForC(std::shared_ptr<Controller> controller)
{
    using enum ItemType;

    controller->regItem<ForC>();


    auto context = controller->context();
    static const auto [lStart, lCodeBlok, lEnd] = context->CodeBlockTypes();

    auto forCRule = controller->addRule("ForCRule", {
       BackusRuleItem({          ForC::Type()}, OnlyOne),
       BackusRuleItem({      Symbols::LBraket}, OnlyOne),
       BackusRuleItem({      "AssignmentRule"}, OnlyOne),
       BackusRuleItem({    Symbols::Semicolon}, OnlyOne),
       BackusRuleItem({ context->EquationRuleName()}, OnlyOne),
       BackusRuleItem({    Symbols::Semicolon}, OnlyOne),
       BackusRuleItem({      "AssignmentRule"}, OnlyOne),
       BackusRuleItem({      Symbols::RBraket}, OnlyOne),
       BackusRuleItem({             lCodeBlok}, OnlyOne)
        });

    forCRule->setPostHandler([context](BackusRuleList::iterator& ruleBegin,
        BackusRuleList::iterator& it,
        BackusRuleList::iterator& end)
        {
            static size_t index = 0;
            index++;

            std::string startLabel = std::format("forStart{}", index);
            std::string endLabel = std::format("forEnd{}", index);
            std::string modifierLabel = std::format("modifier{}", index);
            std::string forBodyLabel = std::format("forBody{}", index);

            (*ruleBegin)->setCustomData(startLabel, "startLabel");
            (*ruleBegin)->setCustomData(endLabel, "endLabel");
            (*ruleBegin)->setCustomData(modifierLabel, "modifierLabel");
            (*ruleBegin)->setCustomData(forBodyLabel, "forbodyLabel");

            size_t count = 0;
            for (auto itr = ruleBegin; itr != it; ++itr)
            {
                auto type = (*itr)->type();
                if (type == lStart)
                {
                    count++;
                }
                else if (type == lEnd && count == 1)
                {
                    (*itr)->setCustomData(std::format("\tjmp {}\n{}:", modifierLabel, endLabel));
                    break;
                }
                else if (type == lEnd && count > 0)
                {
                    count--;
                }
            }
        });

    return forCRule;
}