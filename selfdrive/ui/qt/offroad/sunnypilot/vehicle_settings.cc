#include "selfdrive/ui/qt/offroad/sunnypilot/vehicle_settings.h"
#include "selfdrive/ui/qt/offroad/sunnypilot/collision_detection_settings.h"

#include <tuple>
#include <vector>

#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/qt/widgets/offroad_alerts.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"

VehicleSettings::VehicleSettings(SettingsWindow *parent) : ListWidget(parent) {
  setStyleSheet(R"(
    VehicleSettings {
      background-color: #292929;
      border-radius: 0px;
    }
    * {
      outline: 0;
    }
  )");

  // Header
  QLabel *header = new QLabel(tr("VEHICLE SETTINGS"));
  header->setStyleSheet("QLabel { font-weight: bold; font-size: 24px; color: #ffffff; margin: 20px 0; }");
  addItem(header);

  QLabel *description = new QLabel(tr("Configure vehicle-specific features and safety systems."));
  description->setWordWrap(true);
  description->setStyleSheet("QLabel { font-size: 16px; color: #c9c9c9; margin: 10px 0 30px 0; line-height: 1.5; }");
  addItem(description);

  // Forward Collision Detection Section
  QLabel *collision_header = new QLabel(tr("🚨 FORWARD COLLISION DETECTION"));
  collision_header->setStyleSheet("QLabel { font-weight: bold; font-size: 20px; color: #ffffff; margin: 20px 0 10px 0; }");
  addItem(collision_header);

  // Collision Detection Button that opens the full settings panel
  ButtonControl *collision_btn = new ButtonControl(tr("Configure Collision Detection"), tr("OPEN"));
  QObject::connect(collision_btn, &ButtonControl::clicked, [=]() {
    openCollisionDetectionSettings();
  });
  collision_btn->setDescription(tr("Advanced collision detection settings with graduated thresholds, distance zones, and braking response tuning. Includes Conservative, Balanced, and Sport presets."));
  addItem(collision_btn);

  // Quick Status Display
  auto collision_status = new LabelControl(tr("Current Mode"), "");
  addItem(collision_status);
  
  // Update status display
  updateCollisionStatus();

  // Quick Preset Buttons for immediate access
  QLabel *presets_header = new QLabel(tr("⚡ Quick Presets"));
  presets_header->setStyleSheet("QLabel { font-weight: bold; font-size: 18px; color: #ffffff; margin: 20px 0 10px 0; }");
  addItem(presets_header);

  // Conservative Preset
  ButtonControl *conservative_btn = new ButtonControl(tr("Conservative"), tr("APPLY"));
  QObject::connect(conservative_btn, &ButtonControl::clicked, [=]() {
    applyCollisionPreset("conservative");
    updateCollisionStatus();
  });
  conservative_btn->setDescription(tr("Very early detection, gentle braking - maximum comfort"));
  addItem(conservative_btn);

  // Balanced Preset
  ButtonControl *balanced_btn = new ButtonControl(tr("Balanced"), tr("APPLY"));
  QObject::connect(balanced_btn, &ButtonControl::clicked, [=]() {
    applyCollisionPreset("balanced");
    updateCollisionStatus();
  });
  balanced_btn->setDescription(tr("Default settings - good balance of safety and comfort"));
  addItem(balanced_btn);

  // Sport Preset
  ButtonControl *sport_btn = new ButtonControl(tr("Sport"), tr("APPLY"));
  QObject::connect(sport_btn, &ButtonControl::clicked, [=]() {
    applyCollisionPreset("aggressive");
    updateCollisionStatus();
  });
  sport_btn->setDescription(tr("Later detection, stronger braking - sportier driving feel"));
  addItem(sport_btn);

  // Other vehicle settings can be added here in the future
  QLabel *future_header = new QLabel(tr("🔧 Additional Vehicle Features"));
  future_header->setStyleSheet("QLabel { font-weight: bold; font-size: 18px; color: #c9c9c9; margin: 30px 0 10px 0; }");
  addItem(future_header);

  QLabel *future_note = new QLabel(tr("Additional vehicle-specific features will be added here in future updates."));
  future_note->setWordWrap(true);
  future_note->setStyleSheet("QLabel { font-size: 14px; color: #808080; margin: 10px 0; font-style: italic; }");
  addItem(future_note);
}

void VehicleSettings::openCollisionDetectionSettings() {
  CollisionDetectionSettings *collision_settings = new CollisionDetectionSettings(this->parentWidget());
  collision_settings->setWindowTitle(tr("Forward Collision Detection"));
  collision_settings->setWindowModality(Qt::ApplicationModal);
  collision_settings->show();
  
  // Update our status when the collision settings are changed
  QObject::connect(collision_settings, &CollisionDetectionSettings::presetApplied, [=]() {
    updateCollisionStatus();
  });
}

void VehicleSettings::applyCollisionPreset(const QString &preset) {
  Params params;
  
  if (preset == "conservative") {
    // Very early detection, gentle braking
    params.put("EarlyDetectionConfidence", "50");
    params.put("ModerateDetectionConfidence", "65");
    params.put("AggressiveDetectionConfidence", "85");
    params.put("EarlyDetectionDistance", "100");
    params.put("ModerateDetectionDistance", "60");
    params.put("CloseDetectionDistance", "30");
    params.put("GentleDecelRate", "0.3");
    params.put("ModerateDecelRate", "1.0");
    params.put("AggressiveDecelRate", "2.5");
    params.put("CollisionPresetMode", "Conservative");
    
    ConfirmationDialog::alert(tr("Conservative preset applied!\n\nVery early detection with gentle braking."), this);
    
  } else if (preset == "balanced") {
    // Default balanced settings
    params.put("EarlyDetectionConfidence", "60");
    params.put("ModerateDetectionConfidence", "75");
    params.put("AggressiveDetectionConfidence", "90");
    params.put("EarlyDetectionDistance", "80");
    params.put("ModerateDetectionDistance", "50");
    params.put("CloseDetectionDistance", "25");
    params.put("GentleDecelRate", "0.5");
    params.put("ModerateDecelRate", "1.5");
    params.put("AggressiveDecelRate", "3.0");
    params.put("CollisionPresetMode", "Balanced");
    
    ConfirmationDialog::alert(tr("Balanced preset applied!\n\nGood balance of safety and comfort."), this);
    
  } else if (preset == "aggressive") {
    // Later detection, stronger braking
    params.put("EarlyDetectionConfidence", "70");
    params.put("ModerateDetectionConfidence", "82");
    params.put("AggressiveDetectionConfidence", "93");
    params.put("EarlyDetectionDistance", "65");
    params.put("ModerateDetectionDistance", "40");
    params.put("CloseDetectionDistance", "20");
    params.put("GentleDecelRate", "0.8");
    params.put("ModerateDecelRate", "2.0");
    params.put("AggressiveDecelRate", "3.5");
    params.put("CollisionPresetMode", "Sport");
    
    ConfirmationDialog::alert(tr("Sport preset applied!\n\nLater detection with stronger braking."), this);
  }
}

void VehicleSettings::updateCollisionStatus() {
  Params params;
  QString mode = QString::fromStdString(params.get("CollisionPresetMode"));
  
  if (mode.isEmpty()) {
    mode = "Custom/Unknown";
  }
  
  // Find the status label and update it
  for (int i = 0; i < list->count(); ++i) {
    QListWidgetItem* item = list->item(i);
    QWidget* widget = list->itemWidget(item);
    LabelControl* label_control = qobject_cast<LabelControl*>(widget);
    if (label_control && label_control->text().contains("Current Mode")) {
      label_control->setText(tr("Current Mode: ") + mode);
      break;
    }
  }
} 