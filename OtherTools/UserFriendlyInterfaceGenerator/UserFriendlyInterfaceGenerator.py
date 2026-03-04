file_name = input("Input File Name: ")

with open(file_name) as f:
    lines = f.readlines()
    
    output = ""
    setters = ""
    
    current_category = "Default"
    for line in lines:
        
        if len(line.strip()) == 0: continue
        
        elif line.startswith("#"):
            current_category = line.replace("#", "").replace("\n", "").strip()
        
        else:
            line = line.replace('\n', '')
            type, name, value = line.split(':')
            type = type.strip()
            name = name.strip()
            value = value.strip()
            output += f"\
UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = {'"'}Default|{current_category}{'"'}) \n\
{type} {name} = {value}; \n\n\
\
UFUNCTION(BlueprintCallable, BlueprintPure, Category = {'"'}ScarletMovement|{current_category}{'"'}) \n\
{type} Get{name}() {'{'} return {name}; {'}'} \n\n\
\
UFUNCTION(BlueprintCallable, Category = {'"'}ScarletMovement|{current_category}{'"'})\n\
void Set{name}({type} New{name})\n\
{'{'}\n\
    {name} = New{name};\n\
    Set{type[0].upper() + type[1::]}ParameterValue({'"'}{name}{'"'}, {name}); \n\
{'}'} \n\n\n"

            setters += f"Set{name}({name});\n"
                        
    with open("output.txt", 'w') as of:
        of.write(output)
        of.write("\n\n\n")
        of.write(setters)