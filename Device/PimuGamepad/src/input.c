#include "pimu_gamepad_input.h"

PG12BitVector2 pimu_gamepad_inputs_pack_vector(float x, float y)
{
    if(x < -1)
    {
        x = -1;
    }
    else if(x > 1)
    {
        x = 1;
    }

    if(y < -1)
    {
        y = -1;
    }
    else if(y > 1)
    {
        y = 1;
    }

    PG12BitVector2 result = {
        (uint16_t)((x * 0.5 + 0.5) * 0xFFF),
        (uint16_t)((y * 0.5 + 0.5) * 0xFFF),
    };

    return result;
}

void pimu_gamepad_copy_inputs_5_to_9(PimuGamepadInputReport5* report_5, PimuGamepadInputReport9* report_9)
{
    report_9->buttons.y = report_5->buttons.y;
    report_9->buttons.x = report_5->buttons.x;
    report_9->buttons.b = report_5->buttons.b;
    report_9->buttons.a = report_5->buttons.a;
    report_9->buttons.r = report_5->buttons.r;
    report_9->buttons.zr = report_5->buttons.zr;
    report_9->buttons.minus = report_5->buttons.minus;
    report_9->buttons.plus = report_5->buttons.plus;
    report_9->buttons.stick_right = report_5->buttons.stick_right;
    report_9->buttons.stick_left = report_5->buttons.stick_left;
    report_9->buttons.home = report_5->buttons.home;
    report_9->buttons.capture = report_5->buttons.capture;
    report_9->buttons.chat = report_5->buttons.chat;
    report_9->buttons.dpad_down = report_5->buttons.dpad_down;
    report_9->buttons.dpad_up = report_5->buttons.dpad_up;
    report_9->buttons.dpad_right = report_5->buttons.dpad_right;
    report_9->buttons.dpad_left = report_5->buttons.dpad_left;
    report_9->buttons.l = report_5->buttons.l;
    report_9->buttons.zl = report_5->buttons.zl;
    report_9->buttons.gr = report_5->buttons.gr;
    report_9->buttons.gl = report_5->buttons.gl;

    report_9->left_stick = report_5->left_stick;   
    report_9->right_stick = report_5->right_stick;
}

void pimu_gamepad_copy_inputs_9_to_5(PimuGamepadInputReport9* report_9, PimuGamepadInputReport5* report_5)
{
    report_5->buttons.y = report_9->buttons.y;
    report_5->buttons.x = report_9->buttons.x;
    report_5->buttons.b = report_9->buttons.b;
    report_5->buttons.a = report_9->buttons.a;
    report_5->buttons.r = report_9->buttons.r;
    report_5->buttons.zr = report_9->buttons.zr;
    report_5->buttons.minus = report_9->buttons.minus;
    report_5->buttons.plus = report_9->buttons.plus;
    report_5->buttons.stick_right = report_9->buttons.stick_right;
    report_5->buttons.stick_left = report_9->buttons.stick_left;
    report_5->buttons.home = report_9->buttons.home;
    report_5->buttons.capture = report_9->buttons.capture;
    report_5->buttons.chat = report_9->buttons.chat;
    report_5->buttons.dpad_down = report_9->buttons.dpad_down;
    report_5->buttons.dpad_up = report_9->buttons.dpad_up;
    report_5->buttons.dpad_right = report_9->buttons.dpad_right;
    report_5->buttons.dpad_left = report_9->buttons.dpad_left;
    report_5->buttons.l = report_9->buttons.l;
    report_5->buttons.zl = report_9->buttons.zl;
    report_5->buttons.gr = report_9->buttons.gr;
    report_5->buttons.gl = report_9->buttons.gl;

    report_5->left_stick = report_9->left_stick;   
    report_5->right_stick = report_9->right_stick;
}