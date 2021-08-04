//
//  Header.h
//  example
//
//  Created by saba on 7/6/21.
//

#ifndef Header_h
#define Header_h

namespace Chess {
    static const ShaderID s_SID = { 0xe3,0x21,0x90,0x44,0x42,0x00,0x0f,0xfd,0x2d,0x2f,0xf0,0x46,0x3f,0x8a,0x77,0x21,0x1b,0x82,0xe4,0x61,0xd3,0x97,0xe1,0xb2,0x20,0x8c,0xc9,0x3b,0xb6,0xa1,0x04,0x65 };

#pragma pack (push, 1)
    

    struct Key {
        PubKey m_Account;
        AssetID m_Aid;
    };

    struct Create {
        static const uint32_t s_iMethod = 2;
        AssetID m_Aid;
        Amount m_Amount;
        PubKey game_id;
    };

    struct Join {
        static const uint32_t s_iMethod = 3;
        AssetID m_Aid;
        Amount m_Amount;
        PubKey game_id;
    };
    
    struct Take {
        static const uint32_t s_iMethod = 4;
        PubKey m_Player;
        Amount m_Amount
    };

    struct Move {
        static const uint32_t s_iMethod = 5;
        PubKey m_Player;
        Position present;
        Position future;
        EnPassant* S_enPassant;
        Castling* S_castling;
        Promotion* S_promotion;
    };

    struct AccountData {
        Height m_h0;
        Amount m_Amount;
    };

    struct State{
        Position present;
        Position future;
        EnPassant* S_enPassant;
        Castling* S_castling;
        Promotion* S_promotion;
    };


#pragma pack (pop)
}

#endif /* Header_h */
