#include "../common.h"
#include "../app_common_impl.h"
#include "contract.h"
#include "chess.h"

void DerivePlayerPk(PubKey& pubKey, const ContractID& cid)
{
    Env::DerivePk(pubKey, &cid, sizeof(cid));
}

void create(const ContractID& cid){
    FundsChange fc;
    fc.m_Aid = 0;
    fc.m_Amount = g_DepositCA; 
    fc.m_Consume = 1; //
    Env::GenerateKernel(nullptr, pars.s_iMethod, &pars, sizeof(pars), &fc, 1, nullptr, 0, "generate Chess contract", 0);
}

void create_game(){
}

void join_game(){
    
}

void take(){
    Chess::Take arg;
    DerivePlayerPk(arg.m_Player, cid);

    KeyPlayer key;
    Chess::State bi;

    FundsChange fc;
    fc.m_Aid = sip.m_State.m_Aid; // receive funds of a specific asset type

    sip.get_BidStatus(bi, fc.m_Amount);

    if (!fc.m_Amount)
        return OnError("you lost");

    SigRequest sig;
    sig.m_pID = &cid;
    sig.m_nID = sizeof(cid);

    fc.m_Consume = 0;

    Env::GenerateKernel(&cid, arg.s_iMethod, &arg, sizeof(arg), &fc, 1, &sig, 1, "Take the prize from Chess", 0);
}

void destroy(){}

public static Chess::Game* current_game = NULL;

void makeTheMove(Chess::Position present, Chess::Position future, Chess::EnPassant* S_enPassant, Chess::Castling* S_castling, Chess::Promotion* S_promotion)
{
   char chPiece = current_game->getPieceAtPosition(present.iRow, present.iColumn);

   if ( current_game->isSquareOccupied(future.iRow, future.iColumn) )
   {
      char chAuxPiece = current_game->getPieceAtPosition(future.iRow, future.iColumn);

      if ( Chess::getPieceColor(chPiece) != Chess::getPieceColor(chAuxPiece))
      {
         createNextMessage(Chess::describePiece(chAuxPiece) + " captured!\n");
      }
      else
      {
          return OnError("Error. We should not be making this move");
      }
   }
   else if (true == S_enPassant->bApplied)
   {
      createNextMessage("Pawn captured by \"en passant\" move!\n");
   }

   if ( (true == S_castling->bApplied) )
   {
      createNextMessage("Castling applied!\n");
   }

   current_game->movePiece(present, future, S_enPassant, S_castling, S_promotion, &cid);
}

void movePiece(void, const ContractID& cid)
{
    Chess::Move arg;
   std::string to_record;

   std::string move_from;

   if ( move_from.length() > 2 )
   {
      createNextMessage("You should type only two characters (column and row)\n");
      return;
   }

   Chess::Position present;
   present.iColumn = move_from[0];
   present.iRow    = move_from[1];

   present.iColumn = toupper(present.iColumn);

   if ( present.iColumn < 'A' || present.iColumn > 'H' )
   {
      createNextMessage("Invalid column.\n");
      return;
   }

   if ( present.iRow < '0' || present.iRow > '8' )
   {
      createNextMessage("Invalid row.\n");
      return;
   }

   to_record += present.iColumn;
   to_record += present.iRow;
   to_record += "-";

   present.iColumn = present.iColumn - 'A';

   present.iRow  = present.iRow  - '1';

   char chPiece = current_game->getPieceAtPosition(present.iRow, present.iColumn);
   cout << "Piece is " << char(chPiece) << "\n";

   if ( 0x20 == chPiece )
   {
      createNextMessage("You picked an EMPTY square.\n");
      return;
   }

   if ( Chess::WHITE_PIECE == current_game->getCurrentTurn() )
   {
      if ( false == Chess::isWhitePiece(chPiece) )
      {
         createNextMessage("It is WHITE's turn and you picked a BLACK piece\n");
         return;
      }
   }
   else
   {
      if ( false == Chess::isBlackPiece(chPiece) )
      {
         createNextMessage("It is BLACK's turn and you picked a WHITE piece\n");
         return;
      }
   }

   if ( move_to.length() > 2 )
   {
      createNextMessage("You should type only two characters (column and row)\n");
      return;
   }

   future.iColumn = move_to[0];
   future.iRow    = move_to[1];

   future.iColumn = toupper(future.iColumn);

   if ( future.iColumn < 'A' || future.iColumn > 'H' )
   {
      createNextMessage("Invalid column.\n");
      return;
   }

   if ( future.iRow < '0' || future.iRow > '8' )
   {
      createNextMessage("Invalid row.\n");
      return;
   }

   to_record += future.iColumn;
   to_record += future.iRow;

   future.iColumn = future.iColumn - 'A';


   future.iRow = future.iRow - '1';


   if ( future.iRow == present.iRow && future.iColumn == present.iColumn )
   {
      createNextMessage("[Invalid] You picked the same square!\n");
      return;
   }

   Chess::EnPassant  S_enPassant  = { 0 };
   Chess::Castling   S_castling   = { 0 };
   Chess::Promotion  S_promotion  = { 0 };

   if ( false == isMoveValid(present, future, &S_enPassant, &S_castling, &S_promotion) )
   {
      createNextMessage("[Invalid] Piece can not move to that square!\n");
      return;
   }
   
   if ( S_promotion.bApplied == true )
   {
      std::string piece;
      getline(cin, piece);

      if ( piece.length() > 1 )
      {
         createNextMessage("You should type only one character (Q, R, N or B)\n");
         return;
      }

      char chPromoted = toupper(piece[0]);

      if ( chPromoted != 'Q' && chPromoted != 'R' && chPromoted != 'N' && chPromoted != 'B' )
      {
         createNextMessage("Invalid character.\n");
         return;
      }

      S_promotion.chBefore = current_game->getPieceAtPosition(present.iRow, present.iColumn);

      if (Chess::WHITE_PLAYER == current_game->getCurrentTurn())
      {
         S_promotion.chAfter = toupper(chPromoted);
      }
      else
      {
         S_promotion.chAfter = tolower(chPromoted);
      }

      to_record += '=';
      to_record += toupper(chPromoted);
   }

   current_game->logMove( to_record );

   makeTheMove(present, future, &S_enPassant, &S_castling, &S_promotion);


   if ( true == current_game->playerKingInCheck() )
   {
      if (true == current_game->isCheckMate() )
      {
         if (Chess::WHITE_PLAYER == current_game->getCurrentTurn())
         {
            appendToNextMessage("Checkmate! Black wins the game!\n");
         }
         else
         {
            appendToNextMessage("Checkmate! White wins the game!\n");
         }
      }
      else
      {
         if (Chess::WHITE_PLAYER == current_game->getCurrentTurn())
         {
            appendToNextMessage("White king is in check!\n");
         }
         else
         {
            appendToNextMessage("Black king is in check!\n");
         }
      }
   }
    
    arg.present = present;
    arg.future = future;
    arg.S_enPassant = &S_enPassant;
    arg.S_castling = &S_castling;
    arg.S_promotion = &S_promotion;
    
    
    Env::GenerateKernel(&cid, arg.s_iMethod, &arg, sizeof(arg), nullptr, 0, &sig, 1, "Make a move", 0);
    

   return;
}




