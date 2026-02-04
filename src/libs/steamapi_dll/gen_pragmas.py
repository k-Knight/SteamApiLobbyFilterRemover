import subprocess
import re
import sys

def generate_pragmas(target_dll):
    # The name of your original renamed DLL
    original_dll = "steam_api64_o"
    
    try:
        # Run dumpbin command
        # Ensure you run this script from a Visual Studio Developer Command Prompt
        result = subprocess.check_output(f'dumpbin /exports {target_dll}', shell=True).decode()
        
        # Regex to find the function name in the exports table
        # Columns are usually: ordinal hint RVA name
        export_pattern = re.compile(r'^\s+\d+\s+[0-9A-F]+\s+[0-9A-F]+\s+(\S+)', re.MULTILINE)
        
        matches = export_pattern.findall(result)
        
        if not matches:
            print("// No exports found or dumpbin failed to run.")
            return

        print(f"// Generated pragmas for {target_dll}")
        for func in matches:
            # Skip common non-function lines like 'summary' or header info
            if func in ["ordinal", "hint", "RVA", "name"]:
                continue
                
            print(f'#pragma comment(linker, "/export:{func}={original_dll}.{func}")')

    except subprocess.CalledProcessError:
        print("Error: Make sure 'dumpbin' is in your PATH (use VS Dev Command Prompt).")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python gen_pragmas.py <path_to_dll>")
    else:
        generate_pragmas(sys.argv[1])