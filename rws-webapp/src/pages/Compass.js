import React from 'react';
import styles from './Compass.module.css';

const Compass = ({ direction }) => {
    return (
        <div className={styles.compass}>
            <h2>Wind Direction</h2>
            <div className={styles.degree}>{direction}°</div>
            <div className={styles.face}>
                <span className={styles.label}>N</span>
                <span className={styles.label}>E</span>
                <span className={styles.label}>S</span>
                <span className={styles.label}>W</span>
                <div className={styles.needle} style={{ transform: `rotate(${direction}deg)` }} />
            </div>
        </div>
    );
};

export default Compass;
