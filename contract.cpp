//
//  contract.cpp
//  example
//
//  Created by saba on 7/11/21.
//

#include "../common.h"
#include "../Math.h"
#include "chess.h"

BEAM_EXPORT void Ctor()
{
    
}
BEAM_EXPORT void Dtor(void*)
{
    Env::DelVar_T((uint8_t) 0);
}

BEAM_EXPORT void Method_2(const Chess::Create &c){
    Env::FundsLock(c.m_Aid, c.m_Amount);
    Env::SaveVar_T(c.gameid, c);
}

BEAM_EXPORT void Method_3(const Chess::Join &j){
    //Env::SaveVar_T(j.gameid, ad);
    Chess::Create create;
    Env::LoadVar_T((uint8_t) 0, create);
    Env::Halt_if(j.game_id != create.game_id);
    Env::FundsLock(create.m_Aid, create.m_Amount);
}

BEAM_EXPORT void Method_4(const Chess::Take &t){
    
    Height h = Env::get_Height();
    Chess::AccountData ad;
    bool bLoaded = Env::LoadVar_T(t.m_Key, ad);
    bool bEmpty = !bLoaded;

    if (t.m_Amount)
    {
        if (bEmpty)
        {
            ad.m_h0 = h;
            ad.m_Amount = r.m_Amount;
        }
        else
            Strict::Add(ad.m_Amount, t.m_Amount);

        Env::Halt_if(ad.m_Amount > pars.m_MaxWithdraw);
        Env::SaveVar_T(t.m_Player, ad);

        Env::FundsUnlock(t.m_Player.m_Aid, t.m_Amount); // would fail if not enough funds in the contract
    }
    else
    {
        if (bLoaded && bEmpty)
            Env::DelVar_T(r.m_Key);
    }

    Env::AddSig(t.m_Player);
}

BEAM_EXPORT void Method_5(Chess::Move &m)
{
    Chess::State s;
    Env::LoadVar_T(m.m_Player, s);
    s.present = m.future;
    s.S_enPassant = m.S_enPassant;
    s.S_castling= m.S_castling;
    s.S_promotion = m.S_promotion;
    Env::SaveVar_T(m.m_Player, s);
    Env::AddSig(m.m_Player);
}
