#include <functional>  // for function
#include <memory>      // for make_shared
#include <vector>      // for vector

#include "ftxui/dom/elements.hpp"     // for GraphFunction, Element, graph
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Screen

namespace ftxui {

const wchar_t charset[] = L" ▗▐▖▄▟▌▙█";
const wchar_t charset_empty = L' ';

class Graph : public Node {
 public:
  Graph(GraphFunction graph_function) : graph_function_(graph_function) {}
  ~Graph() override {}

  void ComputeRequirement() override {
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 1;
    requirement_.flex_shrink_x = 1;
    requirement_.flex_shrink_y = 1;
    requirement_.min_x = 3;
    requirement_.min_y = 3;
  }

  void Render(Screen& screen) override {
    int width = (box_.x_max - box_.x_min + 1) * 2;
    int height = (box_.y_max - box_.y_min + 1) * 2;
    auto data = graph_function_(width, height);
    int i = 0;
    for (int x = box_.x_min; x <= box_.x_max; ++x) {
      int height_1 = 2 * box_.y_max - data[i++];
      int height_2 = 2 * box_.y_max - data[i++];
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        int yy = 2 * y;
        int i_1 = yy < height_1 ? 0 : yy == height_1 ? 3 : 6;
        int i_2 = yy < height_2 ? 0 : yy == height_2 ? 1 : 2;
        wchar_t pix = charset[i_1 + i_2];
        screen.at(x, y) = pix;
      }
    }
  }

 private:
  GraphFunction graph_function_;
};

class Graph_line : public Node {
 public:
  Graph_line(GraphFunction graph_function,
             unsigned line_width,
             bool fill = false,
             wchar_t character = L'.')
      : line_width_(line_width),
        fill_(fill),
        character_(character),
        graph_function_(graph_function) {}
  ~Graph_line() override {}

  void ComputeRequirement() override {
    requirement_.flex_grow_x = 1;
    requirement_.flex_grow_y = 1;
    requirement_.flex_shrink_x = 1;
    requirement_.flex_shrink_y = 1;
    requirement_.min_x = 3;
    requirement_.min_y = 3;
  }

  void Render(Screen& screen) override {
    int width = (box_.x_max - box_.x_min + 1);
    int height = (box_.y_max - box_.y_min + 1);
    auto data = graph_function_(width, height);
    unsigned i = 0;
    for (int x = box_.x_min; x <= box_.x_max; ++x) {
      auto value = data[i++];
      auto min_height = box_.y_max - value;
      unsigned drawn = 0;
      for (int y = box_.y_min; y <= box_.y_max; ++y) {
        int yy = y;
        auto should_draw = value && yy >= min_height;
        drawn += should_draw ? 1 : 0;
        wchar_t pix = should_draw && (drawn <= line_width_ || fill_)
                          ? character_
                          : charset_empty;
        screen.at(x, y) = pix;
      }
    }
  }

 private:
  unsigned line_width_;
  bool fill_;
  wchar_t character_;
  GraphFunction graph_function_;
};

/// @brief Draw a graph using a GraphFunction.
/// @param graph_function the function to be called to get the data.
Element graph(GraphFunction graph_function) {
  return std::make_shared<Graph>(graph_function);
}

Element graph_line(GraphFunction graph_function,
                   unsigned line_width,
                   bool fill,
                   wchar_t character) {
  return std::make_shared<Graph_line>(graph_function, line_width, fill,
                                      character);
}
}  // namespace ftxui

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
