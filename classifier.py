from sklearn.svm import SVC
from sklearn.metrics import classification_report
from sklearn.model_selection import train_test_split
import pandas as pd


class Classifier:
    def __init__(self):
        self.data_set = {}
        self.classifier = None

    def create_data_set(self, input_df: pd.DataFrame):
        """ Returns a dictionary of the x and y train and test data. """
        x = input_df.drop("Class", axis=1)
        y = input_df["Class"]

        x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.20)

        self.data_set = {
            "x_train": x_train,
            "x_test": x_test,
            "y_train": y_train,
            "y_test": y_test,
        }

    def train(self):
        """ Returns a SVM classifier object. """
        classifier = SVC(kernel="linear")
        classifier.fit(self.data_set["x_train"], self.data_set["y_train"])
        self.classifier = classifier

    def predict(self, data):
        """ Returns the mode after classifying the data. """
        if self.classifier is None:
            return Exception("Classifier is none. Have you called train yet?")

        mode = self.classifier.predict(data)
        return mode[0]

    def classification_report(self):
        return classification_report(self.data_set["y_test"])

    @staticmethod
    def convert_samples_to_classifiable_csv():
        sample_data = pd.read_csv("fsr-sample-data.csv")
        fsr_readings = pd.DataFrame(columns=["Average", "Class"])
        for i in range(0, sample_data.shape[0], 2):
            off_average = (sample_data["Off"][i] + sample_data["Off"][i + 1]) / 2.0
            low_average = (sample_data["Low"][i] + sample_data["Low"][i + 1]) / 2.0
            medium_average = (
                sample_data["Medium"][i] + sample_data["Medium"][i + 1]
            ) / 2.0
            high_average = (sample_data["High"][i] + sample_data["High"][i + 1]) / 2.0

            rows = [
                {"Average": off_average, "Class": 0},
                {"Average": low_average, "Class": 1},
                {"Average": medium_average, "Class": 2},
                {"Average": high_average, "Class": 3},
            ]

            fsr_readings = fsr_readings.append(rows)
            fsr_readings.to_csv("fsr-classified-averages.csv", index=False)
