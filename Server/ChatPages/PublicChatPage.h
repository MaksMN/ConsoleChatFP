#pragma once
#include "IPagesCore.h"

class PublicChatPage final : public IPagesCore
{
private:
    /* data */
public:
    PublicChatPage(char (&_cmd_buffer)[CMD_BUFFER], DBClient &_dbclient);
    ~PublicChatPage() = default;

    void run() override;
    bool commandHandler() override;
};
