[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 80
  ny = 80
  elem_type = TRI6
[]

[MeshModifiers]
  [./homogen_block]
    type = SubdomainBoundingBox
    top_right = '0.8 0.8 0'
    block_name = homogen
    location = OUTSIDE
    bottom_left = '0.2 0.2 0'
    block_id = 1
  [../]
[]

[Variables]
  [./u]
  [../]
[]

[Functions]
  [./thresh_outside]
    type = ImageFunction
    file_suffix = png
    upper_value = 255
    file_base = stack/anm_many
    lower_value = 254
    threshold = 0
    file_range = 0
  [../]
  [./thresh_inside]
    type = ImageFunction
    file_suffix = png
    upper_value = 175
    file_base = stack/anm_many
    lower_value = 0
    threshold = 100
    file_range = 0
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 0.1
[]

[Adaptivity]
  max_h_level = 2
  initial_steps = 5
  initial_marker = marker
  [./Indicators]
    [./grad_indic]
      type = GradientJumpIndicator
      variable = u
    [../]
  [../]
  [./Markers]
    [./marker]
      type = ErrorFractionMarker
      indicator = grad_indic
      refine = 0.9
    [../]
  [../]
[]

[Outputs]
  exodus = true
  tecplot = true
[]

[ICs]
  [./outside_ic]
    function = thresh_outside
    variable = u
    type = FunctionIC
    block = homogen
  [../]
  [./inside_ic]
    function = thresh_inside
    variable = u
    type = FunctionIC
    block = 0
  [../]
[]

