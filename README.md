# <img src="https://github.com/user-attachments/assets/caabfdf0-0f9e-44a3-8200-c6579fe87887" alt="description icon" width="28"> Description
The project aims to implement an integration with the Gemini Large Language Model (LLM). The implementation is based on creating a question-and-answer (QA) program with the following specifications:
- The user will be able to send questions to the LLM, which will then provide answers to the questions;
- Questions and answers must be saved in a text file in CSV format, which should also record the timestamp of when they occurred;
- A possible text file format is:

```
USER; 05/06/2024 10h00; Qual a capital do Brasil?;
LLM; 05/06/2024 10h01; Brasília;
```
- It must be possible to send multiple questions to the LLM;
- If there is a communication error, the user must be informed, and the error should be recorded in the file:

```
USER; 05/06/2024 10h00; Qual a capital do Brasil?;
LLM; 05/06/2024 10h01; ERRO NA COMUNICAÇÃO;
```

# <sub><img src="https://github.com/user-attachments/assets/2bd91f82-43a7-44c6-8fb3-eaa3ca20089e" alt="terminal icon" width="34"></sub> Example
### Windows Terminal - Interaction
![image](https://github.com/user-attachments/assets/ff80d6d9-807b-4c7e-904d-97a32481ca3d)
<br>
### Save history in a .csv file
![image](https://github.com/user-attachments/assets/d942e8cc-579d-4d6d-a607-237e28ff402a)

