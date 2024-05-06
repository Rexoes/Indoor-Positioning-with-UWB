CREATE TABLE UWB_INDOOR_DATASET (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    TAG_ID VARCHAR(3) NOT NULL,
    TAG_POS_X VARCHAR(3),
    TAG_POS_Y VARCHAR(3),
    TAG_GRID VARCHAR(3),
    A0 VARCHAR(10),
    A1 VARCHAR(10),
    A2 VARCHAR(10),
    A3 VARCHAR(10),
    A4 VARCHAR(10),
    A5 VARCHAR(10),
    A6 VARCHAR(10),
    A7 VARCHAR(10),
    LOCATION VARCHAR(20) NOT NULL,
    READING_TIME TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
)