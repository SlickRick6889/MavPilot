#include "selfdrive/ui/qt/offroad/sunnypilot/collision_detection_settings.h"

#include <tuple>
#include <vector>

#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/qt/widgets/offroad_alerts.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"

CollisionDetectionSettings::CollisionDetectionSettings(SettingsWindow *parent) : ListWidget(parent) {
  setStyleSheet(R"(
    CollisionDetectionSettings {
      background-color: #292929;
      border-radius: 0px;
    }
    * {
      outline: 0;
    }
  )");

  // Header
  QLabel *header = new QLabel(tr("FORWARD COLLISION DETECTION"));
  header->setStyleSheet("QLabel { font-weight: bold; font-size: 24px; color: #ffffff; margin: 20px 0; }");
  addItem(header);

  QLabel *description = new QLabel(tr("Configure collision detection sensitivity and braking behavior. Lower confidence values = earlier detection but may cause false alarms. Higher values = later detection but more confident."));
  description->setWordWrap(true);
  description->setStyleSheet("QLabel { font-size: 16px; color: #c9c9c9; margin: 10px 0 30px 0; line-height: 1.5; }");
  addItem(description);

  // Detection Sensitivity Section
  QLabel *sensitivity_header = new QLabel(tr("🎯 Detection Sensitivity"));
  sensitivity_header->setStyleSheet("QLabel { font-weight: bold; font-size: 20px; color: #ffffff; margin: 20px 0 10px 0; }");
  addItem(sensitivity_header);

  // Early Detection Confidence
  auto early_confidence = new ParamControl("EarlyDetectionConfidence",
                                         tr("Early Detection Confidence"),
                                         tr("Sensitivity for early collision detection at long distances (40-80%)"),
                                         "../assets/offroad/icon_shell.png",
                                         this);
  addItem(early_confidence);

  // Moderate Detection Confidence
  auto moderate_confidence = new ParamControl("ModerateDetectionConfidence",
                                            tr("Moderate Detection Confidence"),
                                            tr("Sensitivity for medium-range collision detection (60-85%)"),
                                            "../assets/offroad/icon_shell.png",
                                            this);
  addItem(moderate_confidence);

  // Aggressive Detection Confidence
  auto aggressive_confidence = new ParamControl("AggressiveDetectionConfidence",
                                              tr("Close-Range Detection Confidence"),
                                              tr("Sensitivity for close-range emergency detection (80-95%)"),
                                              "../assets/offroad/icon_shell.png",
                                              this);
  addItem(aggressive_confidence);

  // Detection Zones Section
  QLabel *zones_header = new QLabel(tr("📏 Detection Distance Zones"));
  zones_header->setStyleSheet("QLabel { font-weight: bold; font-size: 20px; color: #ffffff; margin: 30px 0 10px 0; }");
  addItem(zones_header);

  // Early Detection Distance
  auto early_distance = new ParamControl("EarlyDetectionDistance",
                                       tr("Early Detection Distance"),
                                       tr("Maximum distance for early gentle braking (60-120m)"),
                                       "../assets/offroad/icon_shell.png",
                                       this);
  addItem(early_distance);

  // Moderate Detection Distance
  auto moderate_distance = new ParamControl("ModerateDetectionDistance",
                                          tr("Moderate Detection Distance"),
                                          tr("Distance threshold for moderate braking response (30-70m)"),
                                          "../assets/offroad/icon_shell.png",
                                          this);
  addItem(moderate_distance);

  // Close Detection Distance
  auto close_distance = new ParamControl("CloseDetectionDistance",
                                       tr("Close Detection Distance"),
                                       tr("Distance for aggressive emergency braking (15-35m)"),
                                       "../assets/offroad/icon_shell.png",
                                       this);
  addItem(close_distance);

  // Braking Response Section
  QLabel *braking_header = new QLabel(tr("🚗 Braking Response"));
  braking_header->setStyleSheet("QLabel { font-weight: bold; font-size: 20px; color: #ffffff; margin: 30px 0 10px 0; }");
  addItem(braking_header);

  // Gentle Deceleration Rate
  auto gentle_decel = new ParamControl("GentleDecelRate",
                                     tr("Gentle Deceleration Rate"),
                                     tr("Smooth early braking for long distances (0.3-1.0 m/s²)"),
                                     "../assets/offroad/icon_shell.png",
                                     this);
  addItem(gentle_decel);

  // Moderate Deceleration Rate
  auto moderate_decel = new ParamControl("ModerateDecelRate",
                                       tr("Moderate Deceleration Rate"),
                                       tr("Balanced braking for medium distances (1.0-2.5 m/s²)"),
                                       "../assets/offroad/icon_shell.png",
                                       this);
  addItem(moderate_decel);

  // Aggressive Deceleration Rate
  auto aggressive_decel = new ParamControl("AggressiveDecelRate",
                                         tr("Emergency Deceleration Rate"),
                                         tr("Strong emergency braking for close encounters (2.0-4.0 m/s²)"),
                                         "../assets/offroad/icon_shell.png",
                                         this);
  addItem(aggressive_decel);

  // Preset Configuration Section
  QLabel *presets_header = new QLabel(tr("⚙️ Quick Presets"));
  presets_header->setStyleSheet("QLabel { font-weight: bold; font-size: 20px; color: #ffffff; margin: 30px 0 10px 0; }");
  addItem(presets_header);

  // Conservative Preset
  ButtonControl *conservative_btn = new ButtonControl(tr("Conservative Mode"), tr("APPLY"));
  QObject::connect(conservative_btn, &ButtonControl::clicked, [=]() {
    applyPreset("conservative");
  });
  conservative_btn->setDescription(tr("Very early detection, gentle braking - maximum comfort"));
  addItem(conservative_btn);

  // Balanced Preset
  ButtonControl *balanced_btn = new ButtonControl(tr("Balanced Mode"), tr("APPLY"));
  QObject::connect(balanced_btn, &ButtonControl::clicked, [=]() {
    applyPreset("balanced");
  });
  balanced_btn->setDescription(tr("Default settings - good balance of safety and comfort"));
  addItem(balanced_btn);

  // Aggressive Preset
  ButtonControl *aggressive_btn = new ButtonControl(tr("Sport Mode"), tr("APPLY"));
  QObject::connect(aggressive_btn, &ButtonControl::clicked, [=]() {
    applyPreset("aggressive");
  });
  aggressive_btn->setDescription(tr("Later detection, stronger braking - sportier driving feel"));
  addItem(aggressive_btn);

  // Status/Info Section
  QLabel *info_header = new QLabel(tr("ℹ️ System Status"));
  info_header->setStyleSheet("QLabel { font-weight: bold; font-size: 20px; color: #ffffff; margin: 30px 0 10px 0; }");
  addItem(info_header);

  QLabel *info_text = new QLabel(tr("Changes take effect immediately. Test settings carefully in safe conditions. The system uses radar confidence and distance to determine braking response. Lower confidence = earlier but potentially less accurate detection."));
  info_text->setWordWrap(true);
  info_text->setStyleSheet("QLabel { font-size: 14px; color: #c9c9c9; margin: 10px 0; line-height: 1.4; background-color: #1a1a1a; padding: 15px; border-radius: 8px; }");
  addItem(info_text);
}

void CollisionDetectionSettings::applyPreset(const QString &preset) {
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
    
    ConfirmationDialog::alert(tr("Conservative preset applied successfully!\n\nVery early detection with gentle braking for maximum comfort."), this);
    
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
    
    ConfirmationDialog::alert(tr("Balanced preset applied successfully!\n\nGood balance of safety and driving comfort."), this);
    
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
    
    ConfirmationDialog::alert(tr("Sport preset applied successfully!\n\nLater detection with stronger braking for sportier feel."), this);
  }
  
  // Refresh the UI to show updated values
  emit presetApplied();
} 