#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/offroad/settings.h"
#include "common/params.h"

class CollisionDetectionSettings : public ListWidget {
  Q_OBJECT

public:
  explicit CollisionDetectionSettings(SettingsWindow *parent = 0);

private slots:
  void applyPreset(const QString &preset);

signals:
  void presetApplied();

private:
  Params params;
}; 