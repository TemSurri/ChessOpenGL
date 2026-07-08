#pragma once
#include <vector>
#include <array>
#include <variant>
#include <unordered_map>
#include <cstdint>
#include <bit>
#include <random>
#include <iostream>

class ClassicChess {

	enum PieceTypeBit {
		NO_PIECE,
		W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
		B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
	};

	enum MoveType {
		NORMAL_MOVE,
		PROMOTION_MOVE,
		EN_PASSANT_MOVE,
		CASTLING_MOVE
	};

	struct Move {
		int from;
		int to;

		PieceTypeBit moved;
		PieceTypeBit captured = NO_PIECE;

		MoveType type = NORMAL_MOVE;

		int value = 0;
	};

	struct MoveRecord {
		Move move;

		int oldEnPassantSquare;

		bool oldWhiteCastleKingSide;
		bool oldWhiteCastleQueenSide;
		bool oldBlackCastleKingSide;
		bool oldBlackCastleQueenSide;
	};

	enum OutCome {
		WhiteWin,
		BlackWin,
		Draw,
		Normal
	};

	enum MoveBunch :int {
		TT,
		CAPTURES,
		KM,
		QUIET
	};

	//multi purpose for move batches, TT, captures, killer moves, quiet moves. For orderingmoves in minimax
	struct MoveSet {
		
		
		std::vector<Move> moves;
		
	};

	struct EvaluatedMove {
		// value of minimax after it was searched
		int value;
		Move move;

	};

	private:
		// init 
		bool game = true;
		bool white_upper = true;

		// to change every move
		bool white_move = true;
		int iterator{};

		//curent bitboard implementation

		uint64_t occupancy;
		uint64_t empty;
		uint64_t w_occupancy;
		uint64_t b_occupancy;

		uint64_t w_bishops;
		uint64_t w_pawns;
		uint64_t w_king;
		uint64_t w_rooks;
		uint64_t w_knights;
		uint64_t w_queen;

		uint64_t b_bishops;
		uint64_t b_pawns;
		uint64_t b_king;
		uint64_t b_rooks;
		uint64_t b_knights;
		uint64_t b_queen;

		int en_passant_square = -1;

		bool white_can_castle_kingside = true;
		bool white_can_castle_queenside = true;
		bool black_can_castle_kingside = true;
		bool black_can_castle_queenside = true;

		static constexpr int NORTH = 8;
		static constexpr int SOUTH = -8;

		static constexpr int EAST = 1;
		static constexpr int WEST = -1;

		static constexpr int NORTH_EAST = 9;
		static constexpr int NORTH_WEST = 7;

		static constexpr int SOUTH_EAST = -7;
		static constexpr int SOUTH_WEST = -9;

		uint64_t& get_piece_board(PieceTypeBit piece)
		{
			switch (piece)
			{
			case W_PAWN:   return w_pawns;
			case W_KNIGHT: return w_knights;
			case W_BISHOP: return w_bishops;
			case W_ROOK:   return w_rooks;
			case W_QUEEN:  return w_queen;
			case W_KING:   return w_king;

			case B_PAWN:   return b_pawns;
			case B_KNIGHT: return b_knights;
			case B_BISHOP: return b_bishops;
			case B_ROOK:   return b_rooks;
			case B_QUEEN:  return b_queen;
			case B_KING:   return b_king;

			default:
				throw std::runtime_error("Invalid piece type");
			}
		}

		MoveRecord exec_move(const Move& move);

		void undo_move(const MoveRecord& record);
		
		void updateOccupancy();

		void init_bitboard();

		void print_bitboard();
		
		char piece_to_char(PieceTypeBit piece);
		
		std::string square_to_string(int square);

		void print_moves(const std::vector<Move>& moves);
		
		int inline row_col_to_square(int row, int col);

		//MOVE GENERATION --------------------------------

		PieceTypeBit piece_on_square(int square);

		void add_move(std::vector<Move>& moves, int from, int to, PieceTypeBit moved);	

		bool is_own_piece(int square, bool white);	
		
		bool is_enemy_piece(int square, bool white);
		
		bool is_empty_square(int square);
	
		std::vector<Move> generate_pseudo_moves(bool whiteToMove);

		inline int pop_lsb(uint64_t& bitboard);
		
		inline bool is_set(uint64_t bitboard, int square);

		//pawn logic
		void add_pawn_move(std::vector<Move>& moves, int from, int to, PieceTypeBit pawnType);
		void generate_pawn_moves(std::vector<Move>& moves, bool is_white);

		//precomputed pieces logic

		void generate_knight_moves(std::vector<Move>& moves, bool is_white);
		uint64_t knight_attacks[64];
		// need to init at start of program
		void init_knight_attacks();

		void generate_king_moves(std::vector<Move>& moves, bool is_white);
		uint64_t king_attacks[64];

		// need to init at start of program
		void init_king_attacks();
		
		void add_castling_moves(std::vector<Move>& moves, bool is_white);

		//sliding pieces logic
		inline bool is_valid_slide(int from, int to, int dir);
		
		void generate_sliding_moves(
			std::vector<Move>& moves,
			uint64_t pieces,
			bool is_white,
			PieceTypeBit pieceType,
			const int* dirs,
			int dirCount
		);
		

		void generate_bishop_moves(std::vector<Move>& moves, bool is_white);
		void generate_rook_moves(std::vector<Move>& moves, bool is_white);
		void generate_queen_moves(std::vector<Move>& moves, bool is_white);

		//MOVE FILTERING (LEGAL MOVE GENERATION) ----------------------

		int get_king_square(bool white)
		{
			uint64_t king = white ? w_king : b_king;

			if (!king)
				return -1;

			return std::countr_zero(king);
		}

		bool is_king_in_check(bool whiteKing)
		{
			int kingSquare = get_king_square(whiteKing);

			if (kingSquare == -1)
				return false;

			return is_square_attacked(kingSquare, !whiteKing);
		}

		bool is_square_attacked_by_sliders(int square, bool byWhite);
		bool is_square_attacked(int square, bool byWhite);

		std::vector<Move> generate_legal_moves(bool whiteToMove);


		bool can_be_pinned_to_king(int from, bool white);
		
		
		// MINIMAX && SEARCH OPT -------------------------------------------
		// TT caching | move ordering | killer moves | heuristics/board eval
		uint64_t zobristID[64][2][6];
		uint64_t white_move_key;
		uint64_t random_u64() {
			static std::random_device rd;
			static std::mt19937_64 rng(rd());
			static std::uniform_int_distribution<uint64_t> dist;

			return dist(rng);
		}
		void initZobrist() {
			white_move_key = random_u64();


			for (int spot{}; spot < 64; spot++) {

				for (int color{}; color < 2; color++) {
					for (int piece{}; piece < 6; piece++) {

						zobristID[spot][color][piece] = random_u64();


					}
				}
			}
		}

		bool sameMove(const Move& a,
			const Move& b) const
		{
			return	a.to == b.to &&
					a.from == b.from;
		}

		enum FLAG {
			EXACT,
			UPPER_BOUND,
			LOWER_BOUND
		};

		struct TTEntry {
			uint64_t id;
			bool whitemove;
			Move move;
			int depth;
			int score;
			FLAG bound_type;
		};

	
		int piece_value(PieceTypeBit piece) const;
		int evaluateBoard();


		MoveBunch analyzeMove(Move& move, const Move& TTmove, bool isTT, int depth);
		EvaluatedMove searchRoot(int depth, bool whiteToMove, const std::array<ClassicChess::MoveSet, 4>& legalMoves);
		EvaluatedMove getBestMoveIterative(int maxDepth, bool whiteToMove);
		int minimax(int depth, bool maximizing, int alpha, int beta);
		const int whiteMaximizing = true;
		
		std::array<MoveSet, 4> GenerateOrderedLegalMoves(bool is_white, const Move& TTmove, bool isTT, int depth);
		
		static constexpr size_t TTsize = 1 << 20;
		void hash_piece_board( uint64_t pieces, int color, int pieceIndex, uint64_t& hash);
		uint64_t getHashCode(bool whitemove);
		std::vector< TTEntry> transpositionalTable;
		void cacheEntryTT(TTEntry entry);

		struct SearchStats {
			uint64_t nodes = 0;
			uint64_t leafNodes = 0;
			uint64_t alphaBetaCutoffs = 0;
			uint64_t ttHits = 0;
			uint64_t ttStores = 0;
			uint64_t killerHits = 0;

			double elapsedMs = 0.0;
		};

		SearchStats stats;

		void resetSearchStats();
		void printSearchStats(int depth);

		static constexpr int MAX_SEARCH_DEPTH = 10;

		std::array<std::array<Move, 2>, MAX_SEARCH_DEPTH> killerMoves{};
		std::array<std::array<bool, 2>, MAX_SEARCH_DEPTH> killerValid{};

		bool isCaptureMove(const Move& move) const;
		void storeKillerMove(const Move& move, int depth);
		void clearKillerMoves();

		//GAMELOOP helpers
		
		bool move_turn();
		std::variant<bool, Move> verifyMove(int from, int to);
		bool verifyPick(int r, int c);
		ClassicChess::OutCome calculateState();

	public:

		ClassicChess() :transpositionalTable(TTsize) {
			init_knight_attacks();
			init_king_attacks();
			initZobrist();
		};

		//GAMELOOPS/GAMEMODES
		void gameLoop();
		void gameLoopVSminimaxAI(bool whiteIsAi, int depth);



};