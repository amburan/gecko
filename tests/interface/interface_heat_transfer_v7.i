[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 12
  xmax = 12
  ymax = 3
  zmax = 3
[]

[Variables]
  [./temp]
  [../]
[]

[Kernels]
  [./ms_hc]
    type = MultiscaleHeatConduction
    variable = temp
  [../]
  [./dt]
    type = TimeDerivative
    variable = temp
  [../]
[]

[BCs]
  active = 'top_ms_DC bottom_ms_DC'
  [./left_DC]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 1
  [../]
  [./right_DC]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 0
  [../]
  [./top_ms_DC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = top
    lammps_userobject = lammps_uo
  [../]
  [./bottom_ms_DC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = bottom
    lammps_userobject = lammps_uo
  [../]
[]

[Materials]
  [./hcm]
    type = HeatConductionMaterial
    block = 0
    specific_heat = 1
    thermal_conductivity = 1
  [../]
[]

[UserObjects]
  [./lammps_uo]
    type = LammpsUserObject
    lammpsInput = /home/sthomas/lammps/lammps-ro/examples/COUPLE/simple/in.bar1d_flux
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 9
[]

[Outputs]
  [./out]
    type = Exodus
  [../]
[]

