import numpy as np
from sklearn.cluster import DBSCAN
from sklearn.preprocessing import StandardScaler

from sklearn.cluster import AgglomerativeClustering
from sklearn.metrics import pairwise_distances

def read_points_from_file(filename):
    """Read points from text file."""
    points = []
    ids = []
    neighbors_vectors = []
    neighbor_ids = []
    
    with open(filename, 'r') as file:
        for line in file:
            # Split the line and convert to float
            parts = line.strip().split()
            point_id = int(parts[0])
            x = float(parts[1])
            y = float(parts[2])
                
            points.append([x, y])
            ids.append(point_id)
            
            other_pts = []
            other_ids = []
            for i in range(1, len(parts) // 3):
                other_id = int(parts[3 * i])
                other_x = float(parts[3 * i + 1]) - x
                other_y = float(parts[3 * i + 2]) - y
                other_pts.append([other_x, other_y])
                other_ids.append(other_id)
                
            neighbors_vectors.append(other_pts)
            neighbor_ids.append(other_ids)
    
    return np.array(points), ids, neighbors_vectors, neighbor_ids
    
def save_clustered_points(filename, ids, labels, neighbor_ids, inner_labels_map):
    """Save clustered points to a new file."""
    output_file = filename.replace('.csv', '_clustered.csv')
    
    with open(output_file, 'w') as f:
        for i, point_id in enumerate(ids):
            f.write(f"{point_id},{labels[i]}")
            for j, neighbor_id in enumerate(neighbor_ids[i]):
                f.write(f",{neighbor_id},{inner_labels_map[point_id][j]}")
            f.write("\n")
    
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
    
def directionless_distance(u, v):
    """Treats opposite directions as identical."""
    u = u.flatten()  # Ensure 1D (just in case)
    v = v.flatten()
    cos_sim = np.dot(u, v) / (np.linalg.norm(u) * np.linalg.norm(v) + 1e-12)
    return 1 - abs(cos_sim)

def reshape_to_structure(nested_template, flat_data):
    result = []
    idx = 0
    for sublist in nested_template:
        # Get the length of current sublist
        sublist_length = len(sublist)
        # Extract corresponding elements from flat list
        result.append(flat_data[idx:idx + sublist_length])
        idx += sublist_length
    return result

# Main execution for DBSCAN
if __name__ == "__main__":
    filename = "C:\\Users\\Konstantin\\Desktop\\streetSignalsMoscow.csv"  # Replace with your file name
    
    # Read points
    points, ids, neighbors_vectors, neighbor_ids = read_points_from_file(filename)
    
    # Perform DBSCAN clustering
    eps_mos = 0.006
    eps_aac = 0.666
    labels, scaler = cluster_dbscan(points, eps=eps_mos, min_samples=2)
    
    labels_unique = np.unique(labels)
    idx = np.where(labels_unique == -1)[0]
    if len(idx) > 0:
        labels_unique = np.delete(labels_unique, idx)
        
    inner_labels_map = {}
    for label in labels_unique:
        this_ids = [ids[i] for i in range(len(ids)) if labels[i] == label]
        other_ids = [neighbor_ids[i] for i in range(len(ids)) if labels[i] == label]
        vectors = [neighbors_vectors[i] for i in range(len(ids)) if labels[i] == label]
        vectors = [item for sublist in vectors for item in sublist]
        
        distance_matrix = pairwise_distances(vectors, metric=directionless_distance)
        clustering = AgglomerativeClustering(
            n_clusters=2,
            metric='precomputed',
            linkage='average'
        )
        inner_labels = clustering.fit_predict(distance_matrix)

        inner_labels = reshape_to_structure(other_ids, inner_labels)
        for i in range(0, len(this_ids)):
            inner_labels_map[this_ids[i]] = inner_labels[i]
        
    this_ids = [ids[i] for i in range(len(ids)) if labels[i] == -1]
    other_ids = [neighbor_ids[i] for i in range(len(ids)) if labels[i] == -1]
    vectors = [neighbors_vectors[i] for i in range(len(ids)) if labels[i] == -1]
    for i in range(0, len(this_ids)):
        if len(vectors[i]) == 1:
            inner_labels_map[this_ids[i]] = [0]
            continue
        distance_matrix = pairwise_distances(vectors[i], metric=directionless_distance)
        clustering = AgglomerativeClustering(
            n_clusters=2,
            metric='precomputed',
            linkage='average'
        )
        inner_labels = clustering.fit_predict(distance_matrix)
        inner_labels_map[this_ids[i]] = inner_labels
    x = 909
    
    # Save results
    save_clustered_points(filename, ids, labels, neighbor_ids, inner_labels_map)