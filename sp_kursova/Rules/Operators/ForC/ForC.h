#pragma once
#include "stdafx.h"
#include "Core/Tokens/TokenBase.hpp"
#include "Core/Backus/BackusRuleBase.h"
#include "Core/Generator/GeneratorItemBase.h"

class ForC : public TokenBase<ForC>, public BackusRuleBase<ForC>, public GeneratorItemBase<ForC>
{
    BASE_ITEM

public:
    ForC() { setLexeme("for"); };
    virtual ~ForC() = default;

    void genCode(std::ostream& out, GeneratorDetails& details,
        std::list<std::shared_ptr<IGeneratorItem>>::iterator& it,
        const std::list<std::shared_ptr<IGeneratorItem>>::iterator& end) const final
    {
        it = std::next(it, 3);
        (*it)->genCode(out, details, it, end);
        out << customData("startLabel") << ":" << std::endl;

        it++;
        auto postForm = GeneratorUtils::Instance()->ConvertToPostfixForm(it, end);

        auto postIt = postForm.begin();
        auto postEnd = postForm.end();
        for (const auto& item : postForm)
            item->genCode(out, details, postIt, postEnd);

        out << "\tpop " << details.args().regPrefix << "ax" << std::endl;
        out << "\tcmp " << details.args().regPrefix << "ax, 0" << std::endl;
        out << "\tje " << customData("endLabel") << std::endl;
        out << "\tjmp " << customData("forbodyLabel") << std::endl;
        out << customData("modifierLabel") << ":" << std::endl;

        it++;

        auto endEx = std::find_if(it, end, [](auto item) { return item->type() == "Start"; });

        endEx = std::prev(endEx, 1);

        for (; it != endEx; ++it)
        {
            (*it)->genCode(out, details, it, endEx);
        }

        out << "\tjmp " << customData("startLabel") << std::endl;
        out << customData("forbodyLabel") << ":" << std::endl;

    };
};