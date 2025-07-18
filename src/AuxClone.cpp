// Venom Modules (c) 2023, 2024 Dave Benham
// Licensed under GNU GPLv3

#include "plugin.hpp"
#include "CloneModule.hpp"

#define LIGHT_OFF 0.02f
#define LIGHT_ON  1.f

struct AuxClone : CloneModule {

  AuxClone() {
    venomConfig(EXP_PARAMS_LEN, EXP_INPUTS_LEN, EXP_OUTPUTS_LEN, EXP_LIGHTS_LEN);
    for (int i=0; i<EXPANDER_PORTS; i++) {
      std::string label = string::f("Poly %d", i + 1);
      configInput(EXP_POLY_INPUT+i, label);
      configOutput(EXP_POLY_OUTPUT+i, label);
      configLight(EXP_POLY_LIGHT+i*2, label+" cloned indicator")->description = "yellow: OK, orange: Missing channels, red: Excess channels dropped";
      outputExtensions[EXP_POLY_OUTPUT+i].portNameLink = EXP_POLY_INPUT+i;
      inputExtensions[EXP_POLY_INPUT+i].portNameLink = EXP_POLY_OUTPUT+i;
    }
    configLight(EXP_CONNECT_LIGHT, "Left connection indicator");
  }

  void process(const ProcessArgs& args) override {
    VenomModule::process(args);
  }

  void onExpanderChange(const ExpanderChangeEvent& e) override {
    Module* mod = getLeftExpander().module;
    bool connected = mod && (mod->model==modelCloneMerge || mod->model==modelPolyUnison || mod->model==modelPolyClone);
    lights[EXP_CONNECT_LIGHT].setBrightness( connected );
    if (!connected) {
      for (int i=0; i<EXPANDER_PORTS; i++) {
        outputs[EXP_POLY_OUTPUT+i].setVoltage(0.f);
        outputs[EXP_POLY_OUTPUT+i].setChannels(1);
        lights[EXP_POLY_LIGHT+i*2].setBrightness(0.f);
        lights[EXP_POLY_LIGHT+i*2+1].setBrightness(0.f);
      }
    }
  }  

};

struct AuxCloneWidget : VenomWidget {
  int venomDelCnt = 0;

  AuxCloneWidget(AuxClone* module) {
    setModule(module);
    setVenomPanel("AuxClone");
    float delta=0.f;
    for(int i=0; i<EXPANDER_PORTS; i++){
      addInput(createInputCentered<PolyPort>(Vec(22.5f,61.5f+delta), module, AuxClone::EXP_POLY_INPUT+i));
      addOutput(createOutputCentered<PolyPort>(Vec(22.5f,226.5f+delta), module, AuxClone::EXP_POLY_OUTPUT+i));
      addChild(createLightCentered<SmallLight<VenomYellowRedLight<>>>(Vec(36.f, 214.f+delta), module, AuxClone::EXP_POLY_LIGHT+i*2));
      delta+=35.f;
    }  
    addChild(createLightCentered<SmallSimpleLight<YellowLight>>(Vec(6.f, 10.f), module, AuxClone::EXP_CONNECT_LIGHT));
  }
  
};

Model* modelAuxClone = createModel<AuxClone, AuxCloneWidget>("AuxClone");
