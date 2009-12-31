/*
 * Copyright (c) 2008 Openismus GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <cluttermm.h>
#include <iostream>

namespace
{

typedef struct {
  Glib::RefPtr<Clutter::Actor> actor;
  bool dragging;
  gint relative_handle_x;
  gint relative_handle_y;
} ActorState;

ActorState dragInfo;

static bool on_my_captured_event(Clutter::Event* event, Glib::RefPtr<Clutter::Stage> pStage)
{
  switch(event->type)
  {
  case CLUTTER_MOTION:
    if(dragInfo.dragging && event->type == CLUTTER_MOTION)
    {
      dragInfo.actor->set_position(event->motion.x + dragInfo.relative_handle_x, event->motion.y + dragInfo.relative_handle_y);
      return true;
    }
    return false;
  case CLUTTER_BUTTON_RELEASE:
    dragInfo.dragging = false;
    return true;
  case CLUTTER_BUTTON_PRESS:
    float actor_x, actor_y;

    dragInfo.actor = pStage->get_actor_at_pos(Clutter::PICK_ALL, event->button.x, event->button.y);
    dragInfo.dragging = true;

    dragInfo.actor->get_position(actor_x, actor_y);
    dragInfo.relative_handle_x = actor_x - event->button.x;
    dragInfo.relative_handle_y = actor_y - event->button.y;
    return true;
  default:
    return false;
  }
}


} // anonymous namespace


int main(int argc, char** argv)
{
  Clutter::init(&argc, &argv);
  Clutter::set_motion_events_enabled(false);

  // Get the stage and set its size and color:
  const Glib::RefPtr<Clutter::Stage> stage = Clutter::Stage::get_default();
  stage->set_size(600, 600);
  stage->set_color(Clutter::Color(0x80, 0x80, 0x80, 0xFF)); // gray

  // Add our scrolling container to the stage
  const Glib::RefPtr<Clutter::Group> group = Clutter::Group::create();
  group->set_position(50, 100);
//  group->set_color(Clutter::Color(0x90,0x90,0x90,0xFF));
  stage->add_actor(group);

  stage->signal_captured_event().connect(sigc::bind(&on_my_captured_event, stage));

  ActorState s1, s2, s3;

  // Add some actors to our container:
  {
    s1.dragging = false;
    s1.actor = Clutter::Rectangle::create(Clutter::Color(0x7F, 0xAE, 0xFF, 0xFF));
    s1.actor->set_size(10, 10);
    s1.actor->set_position(0,0);
    group->add_actor(s1.actor);
  }

  {
    s2.dragging = false;
    s2.actor = Clutter::Rectangle::create(Clutter::Color(0xFF, 0x7F, 0xAE, 0xFF));
    s2.actor->set_size(75, 75);
    s2.actor->set_position(-100,-100);
    group->add_actor(s2.actor);
  }

  {
    s3.dragging = false;
    s3.actor = Clutter::Rectangle::create(Clutter::Color(0xAE, 0xFF, 0x7F, 0xFF));
    s3.actor->set_size(400, 400);
    s3.actor->set_position(400,100);
    group->add_actor(s3.actor);
  }

  group->show_all();
  stage->show();

  // Start the main loop, so we can respond to events:
  Clutter::main();

  return 0;
}
