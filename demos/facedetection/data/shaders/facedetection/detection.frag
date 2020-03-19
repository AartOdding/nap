#version 330

// vertex shader input  
in vec3 passUVs;						//< frag Uv's

// Blob detection structure
struct Blob
{
	vec2	mCenter;
	float	mSize;
};

// uniform inputs
uniform Blob blobs[200];				//< All detected blobs
uniform int blobCount;					//< Total number of detected blobs
uniform sampler2D captureTexture;		//< Classify texture 
uniform vec2 captureSize;				//< Size of captureTexture in pixels

const float ringSize = 3;
//const vec3 edgeColor = vec3(0.784,0.411,0.411);
const vec3 edgeColor = vec3(1.0,1.0,1.0);
const vec3 innerColor = vec3(1.0,1.0,1.0);

// output
out vec4 out_Color;


// maps value from min / max to output
float fit(float value, float min, float max, float outMin, float outMax)
{
    float v = clamp(value, min, max);
    if (v == 0.0)
    	v = 0.00001;
    float m = max - min;
    return (v - min) / (m) * (outMax - outMin) + outMin;
}

float bell(float t, float strength)
{
	return pow(4.0f, strength) * pow(t *(1.0f - t), strength);
}


// Computes the distance to a certain blob
float getDistance(int blobID)
{
	// Flip blob vertical
	vec2 center = vec2(blobs[blobID].mCenter.x, captureSize.y - blobs[blobID].mCenter.y); 
	
	// Return distance from pixel to center of blob
	return length(center - (passUVs.xy * captureSize));
}


void main() 
{
	// Iterate over every blob, for every blob compute the distance to current uv coordinate.
	// Store the blob with the lowest distance to current sample. 
	float current_dist = 1000000.0;
	int closest_blob = -1;

	for(int i=0; i < blobCount; i++)
	{
		float blob_dist = getDistance(i);
		if(blob_dist < current_dist)
		{
			closest_blob = i;
			current_dist = blob_dist;
		}
	}

	// For the closest blob, compute lerp values, 1.0 = inside, 0.0 = outside
	float edge_lerp_v = 0.0;
	float inne_lerp_v = 0.0;

	if (closest_blob >= 0)
	{
		// Get blob size and create gradient on edge
		float blob_size = blobs[closest_blob].mSize;
		edge_lerp_v = fit(current_dist, blob_size-ringSize , blob_size+ringSize, 0.0, 1.0);
		edge_lerp_v = bell(edge_lerp_v, 0.33);

		// Compute inner circle
		inne_lerp_v = fit(current_dist, blob_size-ringSize, blob_size, 1.0, 0.0);
	}

	// Mix based on interpolation values
	vec3 tex_color = texture(captureTexture, vec2(passUVs.x, 1.0-passUVs.y)).bgr;
	vec3 mix_color = mix(tex_color, innerColor, inne_lerp_v * 0.5);
	mix_color = mix(mix_color, edgeColor, edge_lerp_v * 1.0);

    //out_Color = vec4(lerp_v, lerp_v, lerp_v, 1.0);
    out_Color = vec4(mix_color, 1.0);
}
