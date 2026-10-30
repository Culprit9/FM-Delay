#include <JuceHeader.h>
#include <iostream>
#include <thread>
#include <atomic>
extern juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
int main(){juce::ScopedJuceInitialiser_GUI init;std::unique_ptr<juce::AudioProcessor> p(createPluginFilter());auto parameter=[&](const char* id){for(auto* q:p->getParameters())if(auto* r=dynamic_cast<juce::RangedAudioParameter*>(q))if(r->getParameterID()==id)return r;return (juce::RangedAudioParameter*)nullptr;};
 std::atomic<bool> running{true};p->prepareToPlay(48000,256);std::thread audio([&]{juce::AudioBuffer<float> b(2,256);juce::MidiBuffer midi;while(running.load()){b.clear();b.setSample(0,0,.1f);b.setSample(1,0,.1f);p->processBlock(b,midi);std::this_thread::yield();}});
 // Reproduce an MS-mode close first, then exercise queued updates in both timing modes.
 for(int n=0;n<250;++n){parameter("sync")->setValueNotifyingHost(n%2?1:0);std::unique_ptr<juce::AudioProcessorEditor> editor(p->createEditor());editor->setVisible(true);if(n%3==0)editor->setSize(843,585);std::thread automation([&]{parameter("time")->setValueNotifyingHost(float(n%91)/100);parameter("analog")->setValueNotifyingHost(float(n%91)/100);});automation.join();if(n%25==0)for(auto* child:editor->getChildren())if(child->getName()=="Note length"&&child->isVisible())dynamic_cast<juce::ComboBox*>(child)->showPopup();editor.reset();if(n%10==0){juce::Thread::sleep(5);juce::Timer::callPendingTimersSynchronously();}}
 running.store(false);audio.join();
 std::cout<<"PASS: 250 editor create/close cycles, MS and Sync, resizing, popup close, pending parameter/timer updates and concurrent audio processing\n";}
