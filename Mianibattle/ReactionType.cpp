#include "ReactionType.h"

const char* ToString(InteractType reaction)
{
    switch (reaction)
    {
    case InteractType::Guard:
        return "Guard";

    case InteractType::Counter:
        return "Counter";

    case InteractType::Parry:
        return "Parry";

    case InteractType::None:
    default:
        return "None";
    }
}
