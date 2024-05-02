#pragma once

#include "gekko_types.h"

#include <memory>
#include <list>
#include <vector>
#include <queue>

namespace Gekko {

	struct NetAddress {
		NetAddress();
		NetAddress(u8* data, u32 size);

		u8* GetAddress();
		u32 GetSize();

		void Copy(NetAddress& other);

	private:
		std::unique_ptr<u8[]> _data;
		u32 _size;
	};

	enum PacketType {
		Inputs = 1,
		SpectatorInputs,
		InputAck,
		SyncRequest,
		SyncResponse,
	};

	struct NetPacket {
		PacketType type;
		u64 magic;

		union data {
			struct Input {
				u32 start_frame;
				u32 input_count;
				u8* inputs;
			} input;
			struct SyncRequest {
				u32 rng_data;
			} sync_request;
			struct SyncResponse {
				u32 rng_data;
			} sync_response;
		}x;
	};

	struct NetData {
		NetAddress addr;
		NetPacket pkt;
	};

	struct NetStats {
		
	};

	class NetAdapter {
	public:
		virtual std::vector<NetData> ReceiveMessages() {};
		virtual void SendMessage(NetAddress& addr, NetPacket& pkt) {};
	};

	enum PlayerType {
		Local,
		Remote,
		Spectator
	};

	enum PlayerStatus {
		Initiating,
		Connected,
		Disconnected,
	};

	class Player
	{
	public:
		Player(Handle phandle, PlayerType type, NetAddress& addr, u64 magic = 0);

		PlayerType GetType();

		PlayerStatus GetStatus();

	public:
		Handle handle;
		u64 session_magic;
		NetAddress address;

	private:
		PlayerType _type;
		PlayerStatus _status;
		NetStats _stats;
	};

	class MessageSystem {
	public:
		MessageSystem();

		void Init(u32 input_size);

		void AddInput(Frame input_frame, u8 input[]);

		void AddSpectatorInput(Frame input_frame, u8 input[]);

		void SendPendingOutput(NetAdapter* host);

		void HandleData(std::vector<NetData>& data);

	public:
		std::vector<Player*> locals;
		std::vector<Player*> remotes;
		std::vector<Player*> spectators;

	private:
		void AddPendingInput(bool spectator = false);

	private:
		static const u32 MAX_PLAYER_SEND_SIZE = 32;
		static const u32 MAX_SPECTATOR_SEND_SIZE = 64;

		u64 _session_magic;

		u32 _input_size;

		Frame _last_added_input;
		Frame _last_added_spectator_input;

		std::list<u8*> _player_input_send_list;
		std::list<u8*> _spectator_input_send_list;

		std::queue<NetData*> _pending_output;
	};
}