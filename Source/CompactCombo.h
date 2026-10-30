#pragma once
#include <JuceHeader.h>
#include "Presets.h"
class CompactCombo : public juce::ComboBox {
 bool notes;
public:
 explicit CompactCombo(bool noteMenu):notes(noteMenu){}
 juce::PopupMenu compactMenu()const{juce::PopupMenu root;if(notes){for(int group=0;group<3;++group){juce::PopupMenu sub;for(int i:fm::noteOrder){juce::String name=fm::noteNames[i];int kind=name.endsWith(" T")?1:name.endsWith(" D")?2:0;if(kind==group)sub.addItem(i+1,name,true,getSelectedId()==i+1);}root.addSubMenu(group==0?"Straight":group==1?"Triplets":"Dotted",sub);}}else{juce::StringArray categories;for(const auto& preset:fm::presets)categories.addIfNotAlreadyThere(preset.category);for(const auto& category:categories){juce::PopupMenu sub;for(int i=0;i<int(fm::presets.size());++i){const auto& preset=fm::presets[i];if(category==preset.category)sub.addItem(i+1,preset.name,true,getSelectedId()==i+1);}root.addSubMenu(category,sub);}}return root;}
 void showPopup()override{auto menu=compactMenu();menu.setLookAndFeel(&getLookAndFeel());auto safe=juce::Component::SafePointer<CompactCombo>(this);menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this).withStandardItemHeight(24).withMinimumWidth(190).withMaximumNumColumns(1),[safe](int id){if(safe!=nullptr){safe->hidePopup();if(id>0)safe->setSelectedId(id,juce::sendNotificationSync);}});}
};
