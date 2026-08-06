#include "ReactionType.h"

const char* ToString(ReactionType reaction)
{
    switch (reaction)
    {
    case ReactionType::Guard:
        return "Guard";

    case ReactionType::Counter:
        return "Counter";

    case ReactionType::Parry:
        return "Parry";

    case ReactionType::None:
    default:
        return "None";
    }
}
