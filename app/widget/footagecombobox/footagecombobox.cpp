/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2020 Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#include "footagecombobox.h"

#include <QAction>
#include <QDebug>

#include "ui/icons/icons.h"
#include "widget/menu/menu.h"

namespace olive {

FootageComboBox::FootageComboBox(QWidget *parent) :
  QComboBox(parent),
  root_(nullptr),
  footage_(nullptr),
  only_show_ready_footage_(true)
{
}

void FootageComboBox::showPopup()
{
  if (root_ == nullptr || root_->item_child_count() == 0) {
    return;
  }

  Menu menu;

  menu.setMinimumWidth(width());

  TraverseFolder(root_, &menu);

  QAction* selected = menu.exec(parentWidget()->mapToGlobal(pos()));

  if (selected != nullptr) {
    SetFootage(selected->data().value<StreamPtr>());

    emit FootageChanged(footage_);
  }
}

void FootageComboBox::SetRoot(const Folder *p)
{
  root_ = p;

  clear();
}

void FootageComboBox::SetOnlyShowReadyFootage(bool e)
{
  only_show_ready_footage_ = e;
}

StreamPtr FootageComboBox::SelectedFootage()
{
  return footage_;
}

void FootageComboBox::SetFootage(StreamPtr f)
{
  // Remove existing single item used to show the footage name
  footage_ = f;

  UpdateText();
}

void FootageComboBox::TraverseFolder(const Folder *f, QMenu *m) const
{
  foreach (Item* child, f->children()) {

    if (child->CanHaveChildren()) {

      Menu* sub = new Menu(child->name(), m);
      m->addMenu(sub);

      TraverseFolder(static_cast<Folder*>(child), sub);

    } else if (child->type() == Item::kFootage) {

      Footage* footage = static_cast<Footage*>(child);

      if (footage->IsValid() || !only_show_ready_footage_) {
        Menu* stream_menu = new Menu(footage->name(), m);
        m->addMenu(stream_menu);

        foreach (StreamPtr stream, footage->streams()) {
          QAction* stream_action = stream_menu->addAction(FootageToString(stream.get()));
          stream_action->setData(QVariant::fromValue(stream));
          stream_action->setIcon(stream->icon());
        }
      }

    }

  }
}

void FootageComboBox::UpdateText()
{
  // Use combobox functions to show the footage name
  clear();

  if (footage_) {
    // Use combobox functions to show the footage name
    addItem(FootageToString(footage_.get()));
  }
}

QString FootageComboBox::FootageToString(Stream *f)
{
  return f->description();
}

}
