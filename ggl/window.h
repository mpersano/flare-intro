#pragma once

#include <vector>

namespace ggl {

class window
{
public:
	window(int width, int height);
	virtual ~window();

	void set_dump_frames(bool b);

	void run();

	virtual void draw(float t) = 0;
	virtual void on_mouse_button_down(int button, int x, int y) { }
	virtual void on_mouse_button_up(int button) { }
	virtual void on_mouse_motion(int x, int y) { }

private:
	bool poll_events();
	void dump_frame();

protected:
	int width_, height_;

private:
	bool dump_frames_;
	int frame_num_;
	std::vector<unsigned char> frame_data_;
};

}
