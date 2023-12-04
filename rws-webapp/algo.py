from flask import Flask, request, jsonify
from flask_cors import CORS
import joblib
import numpy as np
import math

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# Load your models
model1 = joblib.load('models/finalized_model_cedar.sav')
model2 = joblib.load('models/finalized_model_fallElm.sav')
model3 = joblib.load('models/finalized_model_ragweed.sav')
model4 = joblib.load('models/finalized_model_weeds.sav')

@app.route('/predict', methods=['POST'])
def predict():
    data = request.json
    direction_radians = math.radians(data['direction'])

    # Extract features in the order expected by the models
    features = [
        math.cos(direction_radians),
        math.sin(direction_radians),
        data['speed'],
        data['humidity'],
        data['temp'],
        data['pressure'],
        data['rain']
    ]

    # Reshape data to a 2D array-like structure
    features_reshaped = np.array(features).reshape(1, -1)

    # Make predictions using all four models
    prediction1 = model1.predict(features_reshaped)
    prediction2 = model2.predict(features_reshaped)
    prediction3 = model3.predict(features_reshaped)
    prediction4 = model4.predict(features_reshaped)

    # Return all predictions
    return jsonify({
        "Cedar": prediction1.tolist(),
        "Fall Elm": prediction2.tolist(),
        "Ragweed": prediction3.tolist(),
        "Weeds": prediction4.tolist()
    })

if __name__ == '__main__':
    app.run(debug=True)
