#include "input.h"

#include <cstdlib> 
#include <cstring>
#include <iostream>

void Gekko::InputBuffer::Init(u8 delay, u32 input_size)
{
	_input_delay = delay;
	_input_size = input_size;
	_last_received_input = GameInput::NULL_FRAME;

	// init GameInput array
	Input dummy = std::malloc(_input_size);

	if(dummy)
		std::memset(dummy, 0, _input_size);

	for (u32 i = 0; i < BUFF_SIZE; i++) {
		_inputs.push_back(GameInput());
		_inputs[i].Init(GameInput::NULL_FRAME, &dummy, _input_size);
	}

	std::free(dummy);
}

void Gekko::InputBuffer::AddLocalInput(Frame frame, const Input input)
{
	if (_inputs[frame % BUFF_SIZE].frame == GameInput::NULL_FRAME && _input_delay > 0) {
		for (i32 i = 0; i < _input_delay; i++) {
			Input dummy = std::malloc(_input_size);

			if (dummy)
				std::memset(dummy, 0, _input_size);

			AddInput(i,  dummy);

			std::free(dummy);
		}
	}
	AddInput(frame + _input_delay, input);
}

void Gekko::InputBuffer::AddInput(Frame frame, Input input)
{
	if (frame == _last_received_input + 1) {
		_last_received_input++;
		_inputs[frame % BUFF_SIZE].Init(frame, input, _input_size);
	}
}

std::unique_ptr<Gekko::GameInput> Gekko::InputBuffer::GetInput(Frame frame)
{
	std::unique_ptr<GameInput> inp(new GameInput());
	
	inp->frame = GameInput::NULL_FRAME;
	inp->input = nullptr;
	inp->input_len = 0;

	if(_last_received_input < frame)
		return inp;

	if (_inputs[frame % BUFF_SIZE].frame == GameInput::NULL_FRAME)
		return inp;

	inp->Init(_inputs[frame % BUFF_SIZE]);
	return inp;
}

void Gekko::GameInput::Init(GameInput& other)
{
	frame = other.frame;
	input_len = other.input_len;

	if (input) {
		std::memcpy(input, other.input, input_len);
		return;
	}

	input = (Input) std::malloc(input_len);

	if (input)
		std::memcpy(input, other.input, input_len);
}

void Gekko::GameInput::Init(Frame frame_num, Input inp, u32 inp_len)
{
	frame = frame_num;
	input_len = inp_len;

	if (input) {
		std::memcpy(input, inp, input_len);
		return;
	}

	input = (Input) std::malloc(input_len);

	if (input)
		std::memcpy(input, inp, input_len);
}

Gekko::GameInput::~GameInput()
{
	if (input) {
		std::free(input);
		input = nullptr;
	}
}