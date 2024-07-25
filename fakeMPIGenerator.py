import re
import os

head = "#!/bin/bash\n\n"
with open("fakeMPIrun.sh", 'w') as file:
    file.write(head)
run_number = 1

for i in range(1, 57):
    # with open('run_number.txt', 'r') as run_mumber_file:
    #     run_number = run_mumber_file.readline()
    # run_number = int(run_number.strip())
    # updated_number = run_number + 1
    # print(run_number)

    # 生成 Corsika_muon.sh 脚本的 Python 脚本
    # 定义脚本内容
    script_content = "#!/bin/bash\n\n./corsika77500Linux_DPMJET_urqmd < ./all-inputs-files/all-inputs-p" + str(run_number) + " > ./CorsikaRunRecords/DPMJET_urqmd_p" + str(run_number) + "\n\n./corsika77500Linux_DPMJET_urqmd < ./all-inputs-files/all-inputs-He" + str(run_number) + " > ./CorsikaRunRecords/DPMJET_urqmd_He" + str(run_number)

    # 将内容写入文件
    base_dir = os.path.dirname(os.path.abspath(__file__))
    bash_name = "Corsika_muon" + str(run_number) + ".sh"
    bash_path = os.path.join(base_dir, 'CorsikaRunBash', bash_name)

    with open(bash_path, 'w') as file:
        file.write(script_content)

    # print(bash_name + " 脚本已生成")


    # 更新 run_number.txt 文件
    # with open('run_number.txt', 'w') as run_mumber_file:
    #     run_mumber_file.write(str(updated_number))
    # print("run_number.txt 文件已更新")


    # 将生成的脚本加入 fakeMPIrun.sh 文件
    with open('fakeMPIrun.sh', 'r') as file:
        existing_content = file.read()
    new_bash = "bash ./CorsikaRunBash/" + bash_name + " &\n\n"
    with open('fakeMPIrun.sh', 'a') as file:
        file.write(new_bash)

    # 生成all-inputs-p
    file_path_p = os.path.join(base_dir, 'all-inputs-files/all-inputs-p')
    with open(file_path_p, 'r') as filep:
        content = filep.read()
    pInputName = file_path_p + str(run_number)
    with open(pInputName, 'w') as filep:
        filep.write(content)
    pattern = r'(RUNNR\s+)(\d+)'
    new_content = re.sub(pattern, lambda match: f"{match.group(1)}{(run_number - 1) * 2 + 1}", content)
    # 将修改后的内容写回文件
    with open(pInputName, 'w') as filep:
        filep.write(new_content)
    with open(pInputName, 'r') as filep:
        content = filep.read()
    pattern = r'(SEED\s+)(\d+)'
    new_content = re.sub(pattern, lambda match: f"{match.group(1)}{int(match.group(2)) + 7 * (run_number - 1)}", content)
    with open(pInputName, 'w') as filep:
        filep.write(new_content)

    # 生成all-inputs-He
    file_path_He = os.path.join(base_dir, 'all-inputs-files/all-inputs-He')
    with open(file_path_He, 'r') as fileHe:
        content = fileHe.read()
    HeInputName = file_path_He + str(run_number)
    with open(HeInputName, 'w') as fileHe:
        fileHe.write(content)
    pattern = r'(RUNNR\s+)(\d+)'
    new_content = re.sub(pattern, lambda match: f"{match.group(1)}{(run_number - 1) * 2 + 2}", content)
    with open(HeInputName, 'w') as fileHe:
        fileHe.write(new_content)

    with open(HeInputName, 'r') as fileHe:
        content = fileHe.read()
    pattern = r'(SEED\s+)(\d+)'
    new_content = re.sub(pattern, lambda match: f"{match.group(1)}{int(match.group(2)) + 7 * (run_number - 1)}", content)
    with open(HeInputName, 'w') as fileHe:
        fileHe.write(new_content)

    run_number += 1

with open('fakeMPIrun.sh', 'a') as file:
    file.write("wait")
