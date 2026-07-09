from flask import Flask, request, jsonify, send_from_directory, send_file
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from PIL import Image
import os
import math
import base64
import logging

app = Flask(__name__)


output_directory = 'outputs'
if not os.path.exists(output_directory):
    os.makedirs(output_directory)


app.config['UPLOAD_FOLDER'] = 'uploads'


os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

def parse_time_to_ms(time_str):
    h, m, s, ms = map(int, time_str.split(":"))
    return (h * 3600 + m * 60 + s) * 1000 + ms

def is_within_radius_2d(point_a, point_b, radius):
    distance = math.sqrt(
        (point_b["X"] - point_a["X"])**2 +
        (point_b["Y"] - point_a["Y"])**2
    )
    return distance <= radius

@app.route('/download', methods=['GET'])
def download_file():
    try:
        result_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'imagePoint.png')

        with open(result_plot_path, "rb") as image_file:
            encoded_string = base64.b64encode(image_file.read()).decode('utf-8')

        return jsonify({"image": encoded_string}), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/download2', methods=['GET'])
def download_file2():
    try:
        result_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'scanpath.png')

        with open(result_plot_path, "rb") as image_file:
            encoded_string = base64.b64encode(image_file.read()).decode('utf-8')

        return jsonify({"image": encoded_string}), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'wynik' not in request.json or 'photo' not in request.json:
        return jsonify({'error': 'Brak danych plików'}), 400


    file_data_wynik = request.json['wynik']
    file_data_photo = request.json['photo']

    file_name_wynik = request.json.get('filename_wynik', 'wynik_file.txt')
    file_name_photo = request.json.get('filename_photo', 'photo_file.jpg')

    try:

        file_data_bytes_wynik = base64.b64decode(file_data_wynik)
        file_data_bytes_photo = base64.b64decode(file_data_photo)

        file_path_wynik = os.path.join(app.config['UPLOAD_FOLDER'], file_name_wynik)
        with open(file_path_wynik, 'wb') as f1:
            f1.write(file_data_bytes_wynik)

        file_path_photo = os.path.join(app.config['UPLOAD_FOLDER'], file_name_photo)
        with open(file_path_photo, 'wb') as f2:
            f2.write(file_data_bytes_photo)

        imagePoint(file_path_wynik, file_path_photo)

        scanpath(file_path_wynik, file_path_photo)

        return jsonify({'message': 'Pliki zostały przesłane pomyślnie!'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500


def imagePoint(wynik_path, photo_path):

    img = Image.open(photo_path)
    df = pd.read_csv(wynik_path, header=None, sep=',')
    df.columns = ['X', 'Y', 'Z','object', 'dimX', 'dimY', 'locX', 'locY', 'locZ','time']

    df = df[df["object"] == "Obraz"].reset_index(drop=True)
    dimX = int(df['dimX'][1])
    dimY = int(df['dimY'][1])
    locX = int(df['locX'][1])
    locY = int(df['locY'][1])
    locZ = int(df['locZ'][1])

    plt.figure(figsize=(8, 6))
    plt.imshow(img, extent=[locX + (dimX/2), locX - (dimX/2), locZ - (dimY/2), locZ + (dimY/2)], alpha=1)

    plt.scatter(df['X'], df['Z'], alpha=0.1, color='red')

    plt.title('Wykres punktowy z obrazkiem jako tło')
    plt.xticks([])
    plt.yticks([])

    result_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'imagePoint.png')
    plt.savefig(result_plot_path, dpi=300)
    plt.close()

def scanpath(wynik_path, photo_path):

    data = pd.read_csv(wynik_path, header=None)
    data.columns = ['X', 'Y', 'Z', 'object', 'dimX', 'dimY', 'locX', 'locY', 'locZ', 'time']
    data = data[data["object"] == "Obraz"].reset_index(drop=True)
    dimX = int(data['dimX'][1])
    dimY = int(data['dimY'][1])
    locX = int(data['locX'][1])
    locY = int(data['locY'][1])
    locZ = int(data['locZ'][1])

    x = data.X
    z = data.Z

    data['time'] = data['time'].apply(parse_time_to_ms)
    data['time'] = data['time'].diff()
    data['time'] = data['time'].fillna(0)
    time = data.time

    if locY < 0:
        data['X'] = 2*locX -x

    i = 0

    while i < data.shape[0] - 2:
        if is_within_radius_2d(data.iloc[i], data.iloc[i + 1], 3):
            data.loc[i, 'time'] += data.loc[i + 1, 'time']
            data = data.drop(i + 1).reset_index(drop=True)
        else:
            i += 1

    data["row_number"] = range(1, len(data) + 1)
    img = Image.open(photo_path)
    plt.figure(figsize=(16, 12))
    plt.imshow(img,  extent=[locX + (dimX/2), locX - (dimX/2), locZ - (dimY/2), locZ + (dimY/2)], alpha=1)
    for i in range(len(data) - 1):

        plt.scatter(data["X"][i], data["Z"][i], s=data["time"][i], alpha=0.5, color='blue',
                    edgecolor='white', linewidths=0.5)

        plt.plot([data["X"][i], data["X"][i + 1]], [data["Z"][i], data["Z"][i + 1]], color="red", alpha=0.3)

        plt.text(data["X"][i], data["Z"][i], str(i + 1), fontsize=5, ha='center', va='center', color='white')

    plt.scatter(data["X"].iloc[-1], data["Z"].iloc[-1], s=data["time"].iloc[-1], alpha=0.6)

    plt.xticks([])
    plt.yticks([])
    plt.title("Scanpath")

    result_plot_path = os.path.join(app.config['UPLOAD_FOLDER'], 'scanpath.png')
    plt.savefig(result_plot_path, dpi=300)
    plt.close()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

