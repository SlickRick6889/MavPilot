#!/usr/bin/env python3
"""
Quick Forward Collision Detection Tuning Tool
Simple presets and command-line parameter adjustment
"""

import sys
from openpilot.common.params import Params

class CollisionTune:
    def __init__(self):
        self.params = Params()
        
        # Preset configurations
        self.presets = {
            'conservative': {
                'EarlyDetectionConfidence': '50',
                'ModerateDetectionConfidence': '65', 
                'AggressiveDetectionConfidence': '85',
                'EarlyDetectionDistance': '100',
                'ModerateDetectionDistance': '60',
                'CloseDetectionDistance': '30',
                'GentleDecelRate': '0.3',
                'ModerateDecelRate': '1.0',
                'AggressiveDecelRate': '2.5',
                'description': 'Very early detection, gentle braking - maximum comfort'
            },
            'balanced': {
                'EarlyDetectionConfidence': '60',
                'ModerateDetectionConfidence': '75',
                'AggressiveDetectionConfidence': '90', 
                'EarlyDetectionDistance': '80',
                'ModerateDetectionDistance': '50',
                'CloseDetectionDistance': '25',
                'GentleDecelRate': '0.5',
                'ModerateDecelRate': '1.5',
                'AggressiveDecelRate': '3.0',
                'description': 'Default settings - good balance of safety and comfort'
            },
            'aggressive': {
                'EarlyDetectionConfidence': '70',
                'ModerateDetectionConfidence': '82',
                'AggressiveDetectionConfidence': '93',
                'EarlyDetectionDistance': '65',
                'ModerateDetectionDistance': '40',
                'CloseDetectionDistance': '20',
                'GentleDecelRate': '0.8',
                'ModerateDecelRate': '2.0',
                'AggressiveDecelRate': '3.5',
                'description': 'Later detection, stronger braking - sportier feel'
            }
        }
    
    def apply_preset(self, preset_name):
        """Apply a preset configuration"""
        if preset_name not in self.presets:
            print(f"❌ Unknown preset: {preset_name}")
            print(f"Available presets: {', '.join(self.presets.keys())}")
            return False
        
        preset = self.presets[preset_name]
        print(f"🎛️  Applying '{preset_name}' preset...")
        print(f"📋 {preset['description']}")
        
        for param_key, value in preset.items():
            if param_key != 'description':
                try:
                    self.params.put(param_key, value)
                    print(f"✅ {param_key}: {value}")
                except Exception as e:
                    print(f"❌ Error setting {param_key}: {e}")
                    return False
        
        print(f"\n🚗 '{preset_name}' configuration applied successfully!")
        print("💡 Changes take effect immediately - test carefully!")
        return True
    
    def show_current(self):
        """Show current parameter values"""
        print("🎛️  CURRENT FORWARD COLLISION DETECTION SETTINGS")
        print("=" * 55)
        
        params = [
            ('EarlyDetectionConfidence', 'Early Detection Confidence', '%'),
            ('ModerateDetectionConfidence', 'Moderate Detection Confidence', '%'),
            ('AggressiveDetectionConfidence', 'Aggressive Detection Confidence', '%'),
            ('EarlyDetectionDistance', 'Early Detection Distance', 'm'),
            ('ModerateDetectionDistance', 'Moderate Detection Distance', 'm'),
            ('CloseDetectionDistance', 'Close Detection Distance', 'm'),
            ('GentleDecelRate', 'Gentle Deceleration Rate', 'm/s²'),
            ('ModerateDecelRate', 'Moderate Deceleration Rate', 'm/s²'),
            ('AggressiveDecelRate', 'Aggressive Deceleration Rate', 'm/s²'),
        ]
        
        for param_key, name, unit in params:
            try:
                value = self.params.get(param_key, encoding="utf8") or "not set"
                print(f"📊 {name:<30} {value} {unit}")
            except Exception:
                print(f"❌ {name:<30} ERROR")
    
    def set_parameter(self, param_key, value):
        """Set a single parameter"""
        try:
            # Validate the parameter exists
            valid_params = [
                'EarlyDetectionConfidence', 'ModerateDetectionConfidence', 'AggressiveDetectionConfidence',
                'EarlyDetectionDistance', 'ModerateDetectionDistance', 'CloseDetectionDistance', 
                'GentleDecelRate', 'ModerateDecelRate', 'AggressiveDecelRate'
            ]
            
            if param_key not in valid_params:
                print(f"❌ Invalid parameter: {param_key}")
                print(f"Valid parameters: {', '.join(valid_params)}")
                return False
            
            # Validate value is numeric
            float(value)
            
            self.params.put(param_key, str(value))
            print(f"✅ Set {param_key} = {value}")
            return True
            
        except ValueError:
            print(f"❌ Invalid value: {value} (must be numeric)")
            return False
        except Exception as e:
            print(f"❌ Error setting parameter: {e}")
            return False

def show_help():
    """Show usage help"""
    print("🎛️  FORWARD COLLISION DETECTION TUNING TOOL")
    print("=" * 50)
    print("Usage:")
    print("  collision_tune.py preset <conservative|balanced|aggressive>")
    print("  collision_tune.py set <parameter> <value>")
    print("  collision_tune.py show")
    print("  collision_tune.py help")
    print()
    print("Presets:")
    print("  conservative  - Very early detection, gentle braking")
    print("  balanced      - Default settings (recommended)")  
    print("  aggressive    - Later detection, stronger braking")
    print()
    print("Example parameter adjustments:")
    print("  collision_tune.py set EarlyDetectionConfidence 55")
    print("  collision_tune.py set GentleDecelRate 0.4")
    print()
    print("💡 Run 'collision_tune.py show' to see current values")

def main():
    if len(sys.argv) < 2:
        show_help()
        return
    
    tuner = CollisionTune()
    command = sys.argv[1].lower()
    
    if command == 'preset':
        if len(sys.argv) != 3:
            print("❌ Usage: collision_tune.py preset <conservative|balanced|aggressive>")
            return
        tuner.apply_preset(sys.argv[2].lower())
    
    elif command == 'set':
        if len(sys.argv) != 4:
            print("❌ Usage: collision_tune.py set <parameter> <value>")
            return
        tuner.set_parameter(sys.argv[2], sys.argv[3])
    
    elif command == 'show':
        tuner.show_current()
    
    elif command == 'help':
        show_help()
    
    else:
        print(f"❌ Unknown command: {command}")
        show_help()

if __name__ == "__main__":
    main() 