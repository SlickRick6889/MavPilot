#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/offroad/settings.h"
#include "common/params.h"

class VehicleSettings : public ListWidget {
  Q_OBJECT

public:
  explicit VehicleSettings(SettingsWindow *parent = 0);

private slots:
  void openCollisionDetectionSettings();
  void applyCollisionPreset(const QString &preset);
  void updateCollisionStatus();

private:
  Params params;
}; 