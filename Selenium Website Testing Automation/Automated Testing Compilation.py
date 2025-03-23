import subprocess

script1 = r'C:\Users\yegor\OneDrive\Documents\Personal\Selenium\accesscodes_test.py'
script2 = r'C:\Users\yegor\OneDrive\Documents\Personal\Selenium\Account creation.py'

python_interpreter = 'python'

print(f"Running {script1}")
subprocess.run([python_interpreter, script1])

print(f"Running {script2}")
subprocess.run([python_interpreter, script2])

print("Both scripts have been executed.")