file_name = input("Input File Name: ")

with open(file_name) as f:
    lines = f.readlines()
    
    output = ""
    setters = ""
    inputs = ""
    dynamic_gates = ""
    dynamic_gate_initializers = ""
    
    current_category = "Default"
    for line in lines:
        
        if len(line.strip()) == 0: continue
        
        elif line.startswith("/"):
            type, remainder = line.split(":", 1)
            type = type.strip()
            remainder = remainder.strip()
            
            if type == "/Input":
                input_type, input_name = remainder.split(":")
                input_type = input_type.strip()
                input_name = input_name.strip()
                
                inputs += f"\
UFUNCTION(BlueprintCallable, Category = \"ScarletMovement|Input\")\n\
void Set{input_name}Input({input_type} Input) {'{'} Set{input_type[0].upper() + input_type[1::]}InputValue(\"{input_name}\", Input); {'}'}\n\n\
\
UFUNCTION(BlueprintCallable, BlueprintPure, Category = \"ScarletMovement|Input\")\n\
{input_type} Get{input_name}Input() {'{'} return Get{input_type[0].upper() + input_type[1::]}InputValue(\"{input_name}\"); {'}'} \n\n\n"
        
            elif type == "/DynamicGate":
                gate_name, gate_value_name, rule, default_value = remainder.split(":")
                gate_name = gate_name.strip()
                gate_value_name = gate_value_name.strip()
                rule = rule.strip()
                default_value = default_value.strip()
                
                dynamic_gates += f"\
UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = \"Default|DynamicGates\") \n\
bool {gate_value_name} = {default_value}; \n\n\
\
UFUNCTION(BlueprintCallable, Category = \"ScarletMovement|DynamicGates\") \n\
void Set{gate_name}(bool In{gate_value_name}) {'{'} SetDynamicGateNamedValue(\"{gate_name}\", \"{gate_value_name}\", In{gate_value_name}); {'}'} \n\n\
\
UFUNCTION(BlueprintCallable, BlueprintPure, Category = \"ScarletMovement|DynamicGates\") \n\
bool Get{gate_value_name}() {'{'} return GetDynamicGateNamedValue(\"{gate_name}\", \"{gate_value_name}\"); {'}'}\n\n\n"

                dynamic_gate_initializers += f"\
if (!IsDynamicGateValid(\"{gate_name}\"))\n\
    RegisterDynamicGate(\"{gate_name}\", ESCM_DynamicGateRule::{rule}, {default_value});\n\
SetDynamicGateNamedValue(\"{gate_name}\", \"{gate_value_name}\", {gate_value_name});\n\n"
        
        elif line.startswith("#"):
            current_category = line.replace("#", "").replace("\n", "").strip()
        
        else:
            line = line.replace('\n', '')
            type, name, value = line.split(':', 2)
            type = type.strip()
            name = name.strip()
            value = value.strip()
            
            setters += f"Set{name}({name});\n"
            output += f"\
UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = \"Default|{current_category}\") \n\
{type} {name} = {value}; \n\n\
\
UFUNCTION(BlueprintCallable, BlueprintPure, Category = \"ScarletMovement|{current_category}\") \n\
{type} Get{name}() {'{'} return {name}; {'}'} \n\n\
\
UFUNCTION(BlueprintCallable, Category = \"ScarletMovement|{current_category}\")\n\
void Set{name}({type} New{name})\n\
{'{'}\n\
    {name} = New{name};\n\
    Set{type[0].upper() + type[1::]}ParameterValue(\"{name}\", {name}); \n\
{'}'} \n\n\n"
                        
    with open("output.txt", 'w') as of:
        of.write(inputs)
        of.write("\n\n\n")
        of.write(dynamic_gates)
        of.write("\n\n\n")
        of.write(output)
        of.write("\n\n\n")
        of.write(setters)
        of.write("\n\n\n")
        of.write(dynamic_gate_initializers)