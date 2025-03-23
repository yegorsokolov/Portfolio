from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time

driver = webdriver.Chrome()
driver.implicitly_wait(10)
driver.get("https://orderon.tacitdev.ca/")

# Clicking elements based on XPath.
WebDriverWait(driver, 20).until(
    EC.element_to_be_clickable((By.XPATH, "/html/body/app-root/ng-component/article/header/article[2]/section[4]/button[2]/span[5]"))
).click()

WebDriverWait(driver, 20).until(
    EC.element_to_be_clickable((By.XPATH, "//*[@id='mat-menu-panel-0']/div/button"))
).click()

WebDriverWait(driver, 20).until(
    EC.element_to_be_clickable((By.XPATH, "//*[@id='loginForm']/section[3]/button"))
).click()

# Corrected the quote type for the XPath
submit_button = WebDriverWait(driver, 20).until(
    EC.element_to_be_clickable((By.XPATH, '//*[@id="registerForm"]/section[3]/button'))
)

# Function to check for error messages related to fields
def check_for_error_message(field_name):
    error_xpath = f"//mat-error[contains(.,'{field_name} must be entered')]"
    try:
        WebDriverWait(driver, 10).until(EC.visibility_of_element_located((By.XPATH, error_xpath)))
    except Exception:
        print(f"Timeout or other exception occurred while checking the error message for '{field_name}'.")

fields = ["First Name", "Last Name", "Email", "Phone Number", "Zip code", "Birth Month and Date", "Password", "Confirm new password"]

for field in fields:
    check_for_error_message(field)

max_retries = 10
x = 0

while x < max_retries:
    email = f"yegor.sokolov+{x}@tacitcorporation.com"

    for i in range(1, 9): 
        # Correcting XPath to target the input elements directly
        field_xpath = f'//*[@id="registerForm"]/mat-form-field[{i}]/div/div[1]/div[3]/input'
        WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, field_xpath)))
        input_element = driver.find_element(By.XPATH, field_xpath)
        input_element.clear()
        if i == 3: 
            input_element.send_keys(email)
        else:
            input_element.send_keys("Test data")

    register_button_xpath = "//*[@id='registerForm']/section[3]/button"
    WebDriverWait(driver, 20).until(
        EC.element_to_be_clickable((By.XPATH, register_button_xpath))
    ).click()

    time.sleep(5)

    try:
        success_message_xpath = "//h1[contains(text(),'Good night, Test!')]"
        WebDriverWait(driver, 10).until(EC.visibility_of_element_located((By.XPATH, success_message_xpath)))
        print(f"Registration successful with email {email}")
        break
    except Exception as e:
        print(f"Attempt {x + 1} failed for email {email}: {e}")

    x += 1

if x >= max_retries:
    print("Max retries reached. Exiting.")

driver.quit()
