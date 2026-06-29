#!/usr/bin/env python3
"""
Data converter for Industrial Monitoring System
Converts real manufacturing dataset to the format expected by the C++ program
"""

import pandas as pd
import numpy as np
from pathlib import Path

def convert_manufacturing_data(input_file, output_file):
    """
    Convert manufacturing dataset to simplified format
    Selects representative columns for temperature, vibration, signal, latency, and bandwidth
    """
    
    # Read the CSV file
    df = pd.read_csv(input_file)
    
    print(f"Loaded {len(df)} records")
    print(f"Columns: {list(df.columns)}")
    
    # Select representative columns
    # Using actual sensor columns from the manufacturing data
    output_data = []
    
    for idx, row in df.iterrows():
        # Create aggregated metrics from multiple sensors
        # Temperature: average of TE_ (temperature) sensors
        temp_cols = [c for c in df.columns if c.startswith('TE_')]
        temperature = df.loc[idx, temp_cols].mean() if temp_cols else 0
        
        # Vibration: use available pressure sensors as proxy
        pressure_cols = [c for c in df.columns if c.startswith('PT_') or c.startswith('PTCA_')]
        vibration_base = df.loc[idx, pressure_cols].mean() if pressure_cols else 0
        vibration = abs(vibration_base) % 10  # Normalize vibration range
        
        # Signal Strength: use flow data
        flow_cols = [c for c in df.columns if c.startswith('FT_')]
        signal_base = df.loc[idx, flow_cols].mean() if flow_cols else 0
        signal_strength = 30 + (signal_base % 70)  # Map to dBm range (30-100)
        
        # Latency: use pressure valve
        latency = abs(vibration_base % 20) + 0.5  # Map to latency range (0.5-20ms)
        
        # Bandwidth: use air and other flow metrics
        bandwidth = 50 + (abs(signal_base) % 950)  # Map to bandwidth range (50-1000 Mbps)
        
        output_data.append({
            'id': idx + 1,
            'temperature': round(temperature, 2),
            'vibration': round(vibration, 2),
            'signalStrength': round(signal_strength, 2),
            'latency': round(latency, 2),
            'bandwidth': round(bandwidth, 2)
        })
    
    # Write to output file
    output_df = pd.DataFrame(output_data)
    output_df.to_csv(output_file, index=False, sep=' ')
    
    print(f"\nConverted and saved {len(output_data)} records to {output_file}")
    print("\nFirst 5 records:")
    print(output_df.head())
    print("\nData statistics:")
    print(output_df.describe())
    
    return output_df

def create_sample_data(output_file, num_samples=10000):
    """
    Create synthetic but realistic sample data
    """
    np.random.seed(42)
    
    data = {
        'id': np.arange(1, num_samples + 1),
        'temperature': np.random.normal(100, 15, num_samples),  # Mean 100°C, StdDev 15
        'vibration': np.random.exponential(1.5, num_samples),    # Exponential distribution
        'signalStrength': np.random.normal(65, 10, num_samples), # Mean 65 dBm
        'latency': np.random.lognormal(1.5, 0.5, num_samples),   # Log-normal distribution
        'bandwidth': np.random.normal(500, 150, num_samples)     # Mean 500 Mbps
    }
    
    # Ensure values are in realistic ranges
    data['temperature'] = np.clip(data['temperature'], 20, 120)
    data['vibration'] = np.clip(data['vibration'], 0, 5)
    data['signalStrength'] = np.clip(data['signalStrength'], 30, 100)
    data['latency'] = np.clip(data['latency'], 0.5, 20)
    data['bandwidth'] = np.clip(data['bandwidth'], 50, 1000)
    
    df = pd.DataFrame(data)
    df = df.round(2)
    
    df.to_csv(output_file, index=False, sep=' ')
    
    print(f"Created sample data with {num_samples} records")
    print(f"Saved to {output_file}")
    print("\nSample data preview:")
    print(df.head(10))
    
    return df

if __name__ == "__main__":
    # Define paths
    script_dir = Path(__file__).parent
    data_dir = script_dir / "data"
    
    # Create sample data for testing
    sample_output = data_dir / "sample_data.csv"
    print("=== Creating Sample Data ===")
    create_sample_data(str(sample_output), num_samples=10000)
    
    # Convert actual manufacturing data if available
    real_data_input = data_dir / "data.csv"
    if real_data_input.exists():
        real_data_output = data_dir / "data_converted.csv"
        print("\n=== Converting Manufacturing Data ===")
        try:
            convert_manufacturing_data(str(real_data_input), str(real_data_output))
        except Exception as e:
            print(f"Error converting data: {e}")
    else:
        print(f"\n{real_data_input} not found, skipping conversion")
