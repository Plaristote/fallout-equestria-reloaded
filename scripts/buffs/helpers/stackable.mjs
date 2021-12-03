export class StackableBuff {
  constructor(model) {
    this.model = model;
  }
  
  get charges() {
    return this.model.hasVariable("charges") ? this.model.getVariable("charges") : 0;
  }

  set charges(value) {
    this.model.setVariable("charges", value);
  }
  
  initialize() {
    this.charges++;
  }
  
  repeat() {
    this.charges++;
  }
}
