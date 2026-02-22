import numpy as np
from sklearn.cluster import DBSCAN
from sklearn.preprocessing import StandardScaler

def read_points_from_file(filename):
    """Read points from text file."""
    points = []
    ids = []
    
    with open(filename, 'r') as file:
        for line in file:
            # Split the line and convert to float
            parts = line.strip().split()
            if len(parts) == 3:
                point_id = int(parts[0])
                x = float(parts[1])
                y = float(parts[2])
                
                points.append([x, y])
                ids.append(point_id)
    
    return np.array(points), ids
    
def save_clustered_points(filename, points, ids, labels):
    """Save clustered points to a new file."""
    output_file = filename.replace('.txt', '_clustered.txt')
    
    with open(output_file, 'w') as f:
        for i, point_id in enumerate(ids):
            f.write(f"{point_id}\t{points[i, 0]:.6f}\t{points[i, 1]:.6f}\t{labels[i]}\n")
    
    print(f"Clustered data saved to {output_file}")

def cluster_dbscan(points, eps=0.3, min_samples=2):
    """Cluster points using DBSCAN."""
    # Normalize the data
    scaler = StandardScaler()
    points_normalized = scaler.fit_transform(points)
    
    # Apply DBSCAN
    dbscan = DBSCAN(eps=eps, min_samples=min_samples)
    labels = dbscan.fit_predict(points_normalized)
    
    return labels, scaler

# Main execution for DBSCAN
if __name__ == "__main__":
    filename = "path\\to\\file"  # Replace with your file name
    
    # Read points
    points, ids = read_points_from_file(filename)
    
    # Perform DBSCAN clustering
    eps_mos = 0.006
    eps_aac = 0.666
    labels, scaler = cluster_dbscan(points, eps=eps_mos, min_samples=2)\
    
    # Save results
    save_clustered_points(filename, points, ids, labels)