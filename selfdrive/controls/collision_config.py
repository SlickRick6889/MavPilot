#!/usr/bin/env python3
"""
Forward Collision Detection Configuration Tool
Interactive parameter adjustment with visual feedback and safety validation
"""

import os
import sys
import time
from openpilot.common.params import Params

class CollisionConfig:
    def __init__(self):
        self.params = Params()
        self.param_definitions = {
            # Confidence Thresholds (%)
            'EarlyDetectionConfidence': {
                'name': 'Early Detection Confidence',
                'description': 'Sensitivity for early collision detection (lower = more sensitive)',
                'unit': '%',
                'min_val': 40,
                'max_val': 80,
                'default': 60,
                'current': None,
                'step': 5,
                'category': 'Detection Sensitivity'
            },
            'ModerateDetectionConfidence': {
                'name': 'Moderate Detection Confidence', 
                'description': 'Sensitivity for medium-range detection',
                'unit': '%',
                'min_val': 60,
                'max_val': 85,
                'default': 75,
                'current': None,
                'step': 5,
                'category': 'Detection Sensitivity'
            },
            'AggressiveDetectionConfidence': {
                'name': 'Aggressive Detection Confidence',
                'description': 'Sensitivity for close-range detection (original behavior)',
                'unit': '%', 
                'min_val': 80,
                'max_val': 95,
                'default': 90,
                'current': None,
                'step': 5,
                'category': 'Detection Sensitivity'
            },
            
            # Distance Zones (meters)
            'EarlyDetectionDistance': {
                'name': 'Early Detection Distance',
                'description': 'Max distance for early gentle braking',
                'unit': 'm',
                'min_val': 60,
                'max_val': 120,
                'default': 80,
                'current': None,
                'step': 10,
                'category': 'Detection Zones'
            },
            'ModerateDetectionDistance': {
                'name': 'Moderate Detection Distance',
                'description': 'Distance for moderate braking response',
                'unit': 'm',
                'min_val': 30,
                'max_val': 70,
                'default': 50,
                'current': None,
                'step': 5,
                'category': 'Detection Zones'
            },
            'CloseDetectionDistance': {
                'name': 'Close Detection Distance', 
                'description': 'Distance for aggressive braking (emergency)',
                'unit': 'm',
                'min_val': 15,
                'max_val': 35,
                'default': 25,
                'current': None,
                'step': 5,
                'category': 'Detection Zones'
            },
            
            # Deceleration Rates (m/s²)
            'GentleDecelRate': {
                'name': 'Gentle Deceleration Rate',
                'description': 'Early smooth slowdown (far distances)',
                'unit': 'm/s²',
                'min_val': 0.3,
                'max_val': 1.0,
                'default': 0.5,
                'current': None,
                'step': 0.1,
                'category': 'Braking Response'
            },
            'ModerateDecelRate': {
                'name': 'Moderate Deceleration Rate',
                'description': 'Medium braking for medium distances',
                'unit': 'm/s²',
                'min_val': 1.0,
                'max_val': 2.5,
                'default': 1.5,
                'current': None,
                'step': 0.2,
                'category': 'Braking Response'
            },
            'AggressiveDecelRate': {
                'name': 'Aggressive Deceleration Rate',
                'description': 'Emergency braking for close encounters',
                'unit': 'm/s²',
                'min_val': 2.0,
                'max_val': 4.0,
                'default': 3.0,
                'current': None,
                'step': 0.2,
                'category': 'Braking Response'
            }
        }
        self.load_current_values()
    
    def load_current_values(self):
        """Load current parameter values from the system"""
        for param_key, definition in self.param_definitions.items():
            try:
                value = self.params.get(param_key, encoding="utf8")
                if value is not None:
                    definition['current'] = float(value)
                else:
                    definition['current'] = definition['default']
            except (ValueError, TypeError):
                definition['current'] = definition['default']
    
    def save_parameter(self, param_key, value):
        """Save a parameter value to the system"""
        try:
            self.params.put(param_key, str(value))
            self.param_definitions[param_key]['current'] = value
            return True
        except Exception as e:
            print(f"Error saving {param_key}: {e}")
            return False
    
    def get_behavior_description(self, param_key, value):
        """Get a description of how this parameter value affects behavior"""
        definition = self.param_definitions[param_key]
        min_val, max_val = definition['min_val'], definition['max_val']
        
        # Calculate position in range (0-1)
        position = (value - min_val) / (max_val - min_val)
        
        if 'Confidence' in param_key:
            if position < 0.3:
                return "🟢 Very Sensitive - Reacts early, may have false positives"
            elif position < 0.7:
                return "🟡 Balanced - Good mix of safety and comfort" 
            else:
                return "🔴 Conservative - Reacts late, very confident detections only"
        
        elif 'Distance' in param_key:
            if position < 0.3:
                return "🔴 Close Range - Later detection, more aggressive"
            elif position < 0.7:
                return "🟡 Moderate Range - Balanced detection distance"
            else:
                return "🟢 Long Range - Early detection, smoother experience"
        
        elif 'Rate' in param_key:
            if position < 0.3:
                return "🟢 Gentle - Smooth, comfortable braking"
            elif position < 0.7:
                return "🟡 Moderate - Balanced braking response"
            else:
                return "🔴 Aggressive - Strong, immediate braking"
        
        return ""
    
    def display_parameter(self, param_key, show_behavior=True):
        """Display a parameter with its current value and description"""
        definition = self.param_definitions[param_key]
        current = definition['current']
        default = definition['default']
        
        print(f"\n📊 {definition['name']}")
        print(f"   {definition['description']}")
        print(f"   Range: {definition['min_val']}-{definition['max_val']} {definition['unit']}")
        print(f"   Default: {default} {definition['unit']} | Current: {current} {definition['unit']}")
        
        if show_behavior:
            behavior = self.get_behavior_description(param_key, current)
            if behavior:
                print(f"   {behavior}")
        
        # Visual bar
        min_val, max_val = definition['min_val'], definition['max_val']
        position = (current - min_val) / (max_val - min_val)
        bar_length = 20
        filled = int(position * bar_length)
        bar = "█" * filled + "░" * (bar_length - filled)
        print(f"   [{bar}] {current}{definition['unit']}")
    
    def interactive_adjust(self, param_key):
        """Interactive adjustment of a parameter"""
        definition = self.param_definitions[param_key]
        current = definition['current']
        
        while True:
            os.system('clear' if os.name == 'posix' else 'cls')
            print("🎛️  FORWARD COLLISION DETECTION TUNING")
            print("=" * 50)
            
            self.display_parameter(param_key)
            
            print(f"\nAdjust {definition['name']}:")
            print(f"  [+] Increase by {definition['step']}")
            print(f"  [-] Decrease by {definition['step']}")
            print(f"  [d] Reset to default ({definition['default']})")
            print(f"  [s] Save and continue")
            print(f"  [q] Quit without saving")
            
            choice = input("\nChoice: ").lower().strip()
            
            if choice == '+':
                new_value = min(current + definition['step'], definition['max_val'])
                if new_value != current:
                    current = new_value
                    definition['current'] = current
            elif choice == '-':
                new_value = max(current - definition['step'], definition['min_val'])
                if new_value != current:
                    current = new_value
                    definition['current'] = current
            elif choice == 'd':
                current = definition['default']
                definition['current'] = current
            elif choice == 's':
                if self.save_parameter(param_key, current):
                    print(f"✅ Saved {definition['name']}: {current}")
                    time.sleep(1)
                    break
                else:
                    print("❌ Error saving parameter")
                    time.sleep(2)
            elif choice == 'q':
                break
    
    def show_all_parameters(self):
        """Display all parameters grouped by category"""
        categories = {}
        for param_key, definition in self.param_definitions.items():
            category = definition['category']
            if category not in categories:
                categories[category] = []
            categories[category].append(param_key)
        
        print("\n🎛️  FORWARD COLLISION DETECTION - CURRENT SETTINGS")
        print("=" * 60)
        
        for category, param_keys in categories.items():
            print(f"\n📂 {category}")
            print("-" * 30)
            for param_key in param_keys:
                self.display_parameter(param_key, show_behavior=False)
        
        print(f"\n💡 Behavior Summary:")
        for param_key in self.param_definitions:
            definition = self.param_definitions[param_key]
            behavior = self.get_behavior_description(param_key, definition['current'])
            if behavior:
                print(f"   {definition['name']}: {behavior}")
    
    def main_menu(self):
        """Main interactive menu"""
        while True:
            os.system('clear' if os.name == 'posix' else 'cls')
            print("🎛️  FORWARD COLLISION DETECTION CONFIGURATION")
            print("=" * 50)
            print("Configure your collision detection behavior for optimal")
            print("balance between safety and driving comfort.")
            
            self.show_all_parameters()
            
            print(f"\n🔧 Configuration Options:")
            print("  [1-9] Adjust specific parameter")
            print("  [a] Show all current settings")
            print("  [r] Reset all to defaults")
            print("  [q] Quit")
            
            print(f"\nParameters:")
            for i, (param_key, definition) in enumerate(self.param_definitions.items(), 1):
                print(f"  [{i}] {definition['name']}")
            
            choice = input("\nChoice: ").lower().strip()
            
            if choice == 'q':
                break
            elif choice == 'a':
                input("\nPress Enter to continue...")
            elif choice == 'r':
                if input("Reset ALL parameters to defaults? (y/N): ").lower() == 'y':
                    for param_key, definition in self.param_definitions.items():
                        self.save_parameter(param_key, definition['default'])
                    print("✅ All parameters reset to defaults")
                    time.sleep(2)
            elif choice.isdigit():
                param_index = int(choice) - 1
                param_keys = list(self.param_definitions.keys())
                if 0 <= param_index < len(param_keys):
                    self.interactive_adjust(param_keys[param_index])


def main():
    if os.getuid() != 0:
        print("⚠️  This tool should be run as root for parameter changes to take effect.")
        print("   Run with: sudo python3 collision_config.py")
    
    config = CollisionConfig()
    
    if len(sys.argv) > 1:
        if sys.argv[1] == '--show':
            config.show_all_parameters()
        elif sys.argv[1] == '--reset':
            print("Resetting all collision detection parameters to defaults...")
            for param_key, definition in config.param_definitions.items():
                config.save_parameter(param_key, definition['default'])
            print("✅ Reset complete")
    else:
        config.main_menu()


if __name__ == "__main__":
    main() 